#include "http_server.hpp"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>

HttpServer::HttpServer(asio::io_context& io_context, std::uint16_t port)
    : m_context{io_context}
    , m_port{port}
{}

HttpServer::~HttpServer()
{
  std::println("Server stopped.");
}

void HttpServer::add_route(http::verb method, const std::string& path,
                           RequestHandler handler)
{
  if (auto found = m_route_handlers.find(method);
      found != m_route_handlers.end()) {
    found->second.emplace(path, std::move(handler));
  } else {
    std::map<std::string, RequestHandler> map{{path, std::move(handler)}};
    m_route_handlers.emplace(method, std::move(map));
  }
}

void HttpServer::run()
{
  asio::co_spawn(m_context, async_main(), asio::detached);
  std::println("Server started on port {}", m_port);
}

asio::awaitable<void> HttpServer::async_main()
{
  auto executor = co_await asio::this_coro::executor;
  auto acceptor = tcp::acceptor(executor, {tcp::v4(), m_port});
  for (;;) {
    auto socket = co_await acceptor.async_accept();
    asio::co_spawn(executor, handle_client(std::move(socket)), asio::detached);
  }
}

asio::awaitable<void> HttpServer::handle_client(tcp::socket socket)
{
  std::println("New client connected. IP: {}, port: {}\n",
               socket.remote_endpoint().address().to_string(),
               socket.remote_endpoint().port());

  for (;;) {
    boost::beast::flat_buffer buffer;
    http::request_parser<http::string_body> parser;
    auto [ec, _] = co_await http::async_read(
        socket, buffer, parser, asio::as_tuple(asio::use_awaitable));
    auto request = parser.get();
    if (ec) {
      if (ec == http::error::end_of_stream) {
        std::println("\nClient disconnected.");
      }
      break;
    }
    co_await handle_http_request(socket, request);
  }
}

asio::awaitable<void> HttpServer::handle_http_request(tcp::socket& socket,
                                                      HttpRequest& request)
{
  auto const major = request.version() / 10;
  auto const minor = request.version() % 10;
  std::println("{} {} HTTP/{}.{}", request.method_string(), request.target(),
               major, minor);

  for (auto&& header : request) {
    std::println("{}: {}", header.name_string(), header.value());
  }

  http::response<http::string_body> response;

  auto maybe_method = m_route_handlers.find(request.method());
  if (maybe_method != m_route_handlers.end()) {
    auto& method     = maybe_method->second;
    auto maybe_route = method.find(request.target());
    if (maybe_route != method.end()) {
      response = maybe_route->second(request);
    } else {
      response.result(http::status::not_found);
    }
  } else {
    response.result(http::status::not_found);
  }

  response.prepare_payload();
  co_await http::async_write(socket, response);
}
