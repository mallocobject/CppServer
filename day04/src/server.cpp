#include "server.h"
#include "acceptor.h"
#include "connection.h"
#include "event_loop.h"
#include "socket.h"
#include "thread_pool.h"
#include <functional>
#include <unordered_map>

namespace WS
{
Server::Server(EventLoop *loop) : _main_reactor(loop), _acceptor(nullptr), _th_pool(nullptr)
{
    _acceptor = new Acceptor(loop);
    std::function<void(Socket *)> cb =
        std::bind(&Server::handleNewConn, this, std::placeholders::_1);
    _acceptor->setNewConnCallback(cb);

    int size = std::thread::hardware_concurrency();
    _th_pool = new ThreadPool(size);
    for (int i = 0; i < size; i++)
    {
        _vec_sub_reactor.push_back(new EventLoop);
    }

    for (int i = 0; i < size; i++)
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, _vec_sub_reactor[i]);
        _th_pool->addTask(sub_loop);
    }
}

Server::~Server()
{
    if (_acceptor != nullptr)
    {
        delete _acceptor;
        _acceptor = nullptr;
    }
    for (auto p : _map_conns)
    {
        delete p.second;
    }
    _map_conns.clear();
}

void Server::handleNewConn(Socket *sock)
{
    int random = sock->getFd() % _vec_sub_reactor.size();
    Connection *conn = new Connection(_vec_sub_reactor[random], sock);
    std::function<void(Socket *)> del_cb = std::bind(&Server::delConn, this, std::placeholders::_1);
    conn->setDelConnCallback(del_cb);
    _map_conns[sock->getFd()] = conn;
}

void Server::delConn(Socket *sock)
{
    decltype(_map_conns)::iterator iter = _map_conns.find(sock->getFd());
    if (iter != _map_conns.end())
    {
        Connection *conn = iter->second;
        _map_conns.erase(iter);
        delete conn;
        conn = nullptr;
    }
}

} // namespace WS