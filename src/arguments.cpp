#include "arguments.hpp"

#include <print>
#include <string>

Arguments parse_args(int argc, char* argv[])
{
  Arguments args;
  for (int i = 1; i < argc; ++i) {
    std::string flag{argv[i]};
    if (flag == "--port" || flag == "-p") {
      args.port = static_cast<std::uint16_t>(std::stoi(argv[i + 1]));
      ++i;
    }
    if (flag == "--help" || flag == "-h") {
      std::println("Usage: http-server [options]");
      std::println("Options:");
      std::println("  -p, --port <port>  Port to listen on (default: 6565)");
      std::println("  -h, --help         Show this help message");
      exit(EXIT_SUCCESS);
    }
  }
  return args;
}
