#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/beast/http.hpp>

#include <cstdint>
#include <functional>
#include <map>

namespace asio = boost::asio;
namespace http = boost::beast::http;
using boost::asio::ip::tcp;

using HttpRequest    = http::request<http::string_body>;
using HttpResponse   = http::response<http::string_body>;
using RequestHandler = std::function<HttpResponse(HttpRequest&)>;
using RouteHandlers  = std::map<std::string, RequestHandler>;

class HttpServer
{
  asio::io_context& m_context;
  std::uint16_t m_port;
  std::map<http::verb, RouteHandlers> m_route_handlers;

  asio::awaitable<void> async_main();
  asio::awaitable<void> handle_client(tcp::socket);
  asio::awaitable<void> handle_http_request(tcp::socket&, HttpRequest&);

 public:
  HttpServer(asio::io_context& io_context, std::uint16_t port);
  ~HttpServer();

  void add_route(http::verb, const std::string& path, RequestHandler handler);
  void run();
};

#endif // SERVER_HPP
