#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <cstdint>

struct Arguments
{
  std::uint16_t port = 6565;
};

Arguments parse_args(int argc, char* argv[]);

#endif // ARGUMENTS_H
