#include "server.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>

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

asio::awaitable<void> Server::handle_client(tcp::socket socket)
{
  std::println("New client connected. IP: {}, port: {}",
               socket.remote_endpoint().address().to_string(),
               socket.remote_endpoint().port());

  asio::streambuf buffer;
  for (;;) {
    co_await asio::async_read_until(socket, buffer, '\n');
    auto data = buffer.data();
    std::string_view str{static_cast<char const*>(data.data()), data.size()};
    std::print("{}", str);
    co_await asio::async_write(socket, asio::buffer(str));
    buffer.consume(buffer.size());
  }
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
