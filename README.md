# Minimal HTTP server

Minimal C++ HTTP server using boost::asio and C++ 20's coroutines.

## Requirements

- vcpkg
- C++ 23 compiler

## Configure and build

```shell
cmake -S . -B build
cmake --build build
```

## Run

```shell
./build/http-server -h
Usage: http-server [options]
Options:
  -p, --port <port>  Port to listen on (default: 6565)
  -h, --help         Show this help message
```

## Acknowledgements

Inspired by [https://github.com/rsreds/fastleaderboard](https://github.com/rsreds/fastleaderboard/tree/main).
