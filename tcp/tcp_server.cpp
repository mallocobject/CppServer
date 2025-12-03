#include "tcp_server.h"
#include "acceptor.h"
#include "event_loop.h"
#include "event_loop_thread_pool.h"
#include "tcp_connection.h"
#include <cassert>
#include <cstdio>
#include <functional>
#include <memory>
#include <unistd.h>
#include <unordered_map>

namespace WS
{
TcpServer::TcpServer(const char *ip, uint16_t port) : _next_id(1)
{
    _main_reactor = std::make_unique<EventLoop>();
    _acceptor = std::make_unique<Acceptor>(_main_reactor.get(), ip, port);
    std::function<void(int)> cb =
        std::bind(&TcpServer::handleNewConnection, this, std::placeholders::_1);
    _acceptor->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency();
    // size = 1;
    _sub_reactor_pool = std::make_unique<EventLoopThreadPool>(_main_reactor.get(), size);
}

TcpServer::~TcpServer()
{
}

void TcpServer::startup()
{
    _sub_reactor_pool->startup();
    _main_reactor->loop();
}

void TcpServer::handleNewConnection(int fd)
{
    assert(fd != -1);
    auto conn = std::make_shared<TcpConnection>(_sub_reactor_pool->getNextLoop(), fd, _next_id++);
    std::function<void(const std::shared_ptr<TcpConnection> &)> cb =
        std::bind(&TcpServer::handleClose, this, std::placeholders::_1);
    conn->setCloseCallback(cb);
    conn->setConnectionCallback(_on_connect_cb);
    conn->setMessageCallback(_on_message_cb);
    _map_conns[fd] = conn;

    conn->connecntionEstablished();
}

void TcpServer::handleClose(const std::shared_ptr<TcpConnection> &conn)
{
    _main_reactor->runOneFunc(std::bind(&TcpServer::handleCloseInLoop, this, conn));
}

void TcpServer::handleCloseInLoop(const std::shared_ptr<TcpConnection> &conn)
{
    decltype(_map_conns)::iterator iter = _map_conns.find(conn->getFd());
    assert(iter != _map_conns.end());
    _map_conns.erase(iter);
    // delete conn;
    conn->getEventLoop()->queueOneFunc(std::bind(&TcpConnection::connectionDestructor, conn));
}

} // namespace WS