#include "arguments.hpp"
#include "server.hpp"

void add_routes(Server& server)
{
  server.add_route(http::verb::get, "/hello", [](HttpRequest& req) {
    HttpResponse res{http::status::ok, req.version()};
    res.body() = "Hello, World!\n";
    return res;
  });

  server.add_route(http::verb::post, "/hello", [](HttpRequest& req) {
    std::println("{}", req.body());
    HttpResponse res{http::status::ok, req.version()};
    res.body() = "done!\n";
    return res;
  });
}

int main(int argc, char* argv[])
{
  auto const args = parse_args(argc, argv);
  asio::io_context io_context;
  Server server{io_context, args.port};
  add_routes(server);
  server.run();
  io_context.run();
  return EXIT_SUCCESS;
}
