#include "arguments.hpp"
#include "http_server.hpp"

void add_routes(HttpServer& server)
{
  server.add_route(http::verb::get, "/hello", [](HttpRequest& req) {
    return HttpResponse{http::status::ok, req.version(), "Hello, World!\n"};
  });

  server.add_route(http::verb::post, "/hello", [](HttpRequest& req) {
    std::println("{}", req.body());
    return HttpResponse{http::status::ok, req.version(), "done!\n"};
  });
}

int main(int argc, char* argv[])
{
  auto const args = parse_args(argc, argv);
  asio::io_context io_context;
  HttpServer server{io_context, args.port};
  add_routes(server);
  server.run();
  io_context.run();
  return EXIT_SUCCESS;
}
