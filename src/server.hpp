#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <cstdint>

namespace asio = boost::asio;
using boost::asio::ip::tcp;

class Server
{
  asio::io_context& m_context;
  std::uint16_t m_port;

  asio::awaitable<void> async_main();
  asio::awaitable<void> handle_client(tcp::socket socket);

 public:
  Server(asio::io_context& io_context, std::uint16_t port);
  ~Server();
};

#endif // SERVER_HPP
