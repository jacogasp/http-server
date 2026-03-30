#include "server.hpp"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

Server::Server(asio::io_context& io_context, std::uint16_t port)
    : m_context{io_context}
    , m_port{port}
{
  asio::co_spawn(io_context, async_main(), asio::detached);
  std::println("Server started on port {}", m_port);
}

Server::~Server()
{
  std::println("Server stopped.");
}

asio::awaitable<void> Server::async_main()
{
  auto executor = co_await asio::this_coro::executor;
  auto acceptor = tcp::acceptor(executor, {tcp::v4(), m_port});
  for (;;) {
    auto socket = co_await acceptor.async_accept();
    asio::co_spawn(executor, handle_client(std::move(socket)), asio::detached);
  }
}

asio::awaitable<void>
handle_http_request(http::request<http::string_body>& request,
                    tcp::socket& socket)
{
  auto const major = request.version() / 10;
  auto const minor = request.version() % 10;
  std::println("{} {} HTTP/{}.{}", request.method_string(), request.target(),
               major, minor);

  for (auto&& header : request) {
    std::println("{}: {}", header.name_string(), header.value());
  }

  http::response<http::string_body> response;

  switch (request.method()) {
  case http::verb::get: {
    if (request.target() == "/hello") {
      response.result(http::status::ok);
      response.body() = "ciao!";
      break;
    }
    response.result(http::status::not_found);
  } break;
  default:
    response.result(http::status::not_found);
    break;
  }
  response.prepare_payload();
  co_await http::async_write(socket, response);
}

asio::awaitable<void> Server::handle_client(tcp::socket socket)
{
  std::println("New client connected. IP: {}, port: {}",
               socket.remote_endpoint().address().to_string(),
               socket.remote_endpoint().port());
  boost::beast::flat_buffer buffer;
  http::request_parser<http::string_body> parser;
  for (;;) {
    auto [ec, size] = co_await http::async_read(
        socket, buffer, parser, asio::as_tuple(asio::use_awaitable));
    auto request = parser.get();
    if (ec) {
      if (ec == http::error::end_of_stream) {
        std::println("\nClient disconnected.");
      }
      break;
    }
    co_await handle_http_request(request, socket);
  }
}
