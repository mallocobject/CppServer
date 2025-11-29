#ifndef __SERVER_H__
#define __SERVER_H__

#include <unordered_map>
#include <vector>
namespace WS
{
class Socket;
class Acceptor;
class Connection;
class EventLoop;
class ThreadPool;
class Server
{
  protected:
    EventLoop *_main_reactor;
    Acceptor *_acceptor;
    std::unordered_map<int, Connection *> _map_conns;
    std::vector<EventLoop *> _vec_sub_reactor;
    ThreadPool *_th_pool;

  public:
    Server(EventLoop *loop);
    ~Server();

    void handleNewConn(Socket *sock);
    void delConn(Socket *sock);
};
} // namespace WS

#endif