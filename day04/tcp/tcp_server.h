#ifndef __TcpServer_H__
#define __TcpServer_H__

#include "common.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
namespace WS
{
class Acceptor;
class EventLoop;
class ThreadPool;
class TcpConnection;
class TcpServer
{
  protected:
    std::unique_ptr<EventLoop> _main_reactor;
    std::unique_ptr<Acceptor> _acceptor;
    std::unordered_map<int, std::shared_ptr<TcpConnection>> _map_conns;
    std::vector<std::unique_ptr<EventLoop>> _vec_sub_reactors;
    std::unique_ptr<ThreadPool> _th_pool;
    std::function<void(const std::shared_ptr<TcpConnection> &)> _on_connect_cb;
    std::function<void(const std::shared_ptr<TcpConnection> &)> _on_message_cb;
    int _next_id;

  public:
    DISALLOW_DEFAULT(TcpServer)
    DISALLOW_COPY_AND_MOVE(TcpServer)

    TcpServer(const char *ip, uint16_t port);
    ~TcpServer();

    void start();

    void setConnectionCallback(
        const std::function<void(const std::shared_ptr<TcpConnection> &)> &cb)
    {
        _on_connect_cb = std::move(cb);
    }

    void setMessageCallback(const std::function<void(const std::shared_ptr<TcpConnection> &)> &cb)
    {
        _on_message_cb = std::move(cb);
    }

    void handleNewConnection(int fd);
    void handleClose(const std::shared_ptr<TcpConnection> &conn);
    void handleCloseInLoop(const std::shared_ptr<TcpConnection> &conn);
};
} // namespace WS

#endif