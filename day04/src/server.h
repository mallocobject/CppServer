#ifndef __SERVER_H__
#define __SERVER_H__

#include <unordered_map>
namespace WS
{
class Socket;
class Acceptor;
class Connection;
class EventLoop;
class Server
{
  protected:
    EventLoop *_loop;
    Acceptor *_acceptor;
    std::unordered_map<int, Connection *> _map_conns;

  public:
    Server(EventLoop *loop);
    ~Server();

    void handleNewConn(Socket *sock);
    void delConn(Socket *sock);
};
} // namespace WS

#endif