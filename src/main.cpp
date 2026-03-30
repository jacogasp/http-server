#include "server.hpp"

struct Args
{
  std::uint16_t port = 6565;
};

Args parse_args(int argc, char* argv[])
{
  Args args;
  for (int i = 1; i < argc; ++i) {
    std::string flag{argv[i]};
    if (flag == "--port" || flag == "-p") {
      args.port = static_cast<std::uint16_t>(std::stoi(argv[i + 1]));
      ++i;
    }
  }
  return args;
}

int main(int argc, char* argv[])
{
  auto const args = parse_args(argc, argv);
  asio::io_context io_context;
  Server server{io_context, args.port};
  io_context.run();
  return EXIT_SUCCESS;
}
