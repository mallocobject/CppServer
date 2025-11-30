#include "tcp_server.h"
#include "acceptor.h"
#include "event_loop.h"
#include "tcp_connection.h"
#include "thread_pool.h"
#include <cassert>
#include <cstdio>
#include <functional>
#include <memory>
#include <unistd.h>
#include <unordered_map>
#include <utility>

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
    _th_pool = std::make_unique<ThreadPool>(size);
    for (int i = 0; i < size; i++)
    {
        auto sub_reactor = std::make_unique<EventLoop>();
        _vec_sub_reactors.push_back(std::move(sub_reactor));
    }
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    for (int i = 0; i < _vec_sub_reactors.size(); i++)
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, _vec_sub_reactors[i].get());
        _th_pool->addTask(std::move(sub_loop));
    }
    _main_reactor->loop();
}

void TcpServer::handleNewConnection(int fd)
{
    assert(fd != -1);
    int random = fd % _vec_sub_reactors.size();
    TcpConnection *conn = new TcpConnection(_vec_sub_reactors[random].get(), fd, _next_id++);
    std::function<void(int)> cb = std::bind(&TcpServer::handleClose, this, std::placeholders::_1);
    conn->setCloseCallback(cb);
    conn->setMessageCallback(_on_message_cb);
    // conn->setConnectionCallback(_on_connect_cb);
    _map_conns[fd] = conn;
}

void TcpServer::handleClose(int fd)
{
    decltype(_map_conns)::iterator iter = _map_conns.find(fd);
    assert(iter != _map_conns.end());

    TcpConnection *conn = iter->second;
    _map_conns.erase(iter);
    // delete conn;
    conn = nullptr;
    ::close(fd);
}

} // namespace WS