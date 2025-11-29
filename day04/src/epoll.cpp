#include "epoll.h"
#include "channel.h"
#include "utils.h"
#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

#define MAX_EVENTS 1024

namespace WS
{
Epoll::Epoll()
{
    _epfd = epoll_create1(0);
    erro(_epfd == -1, "epoll create failed");
    bzero(&_ev, sizeof(epoll_event));
    _events = new epoll_event[MAX_EVENTS];
    _vec_chs.resize(MAX_EVENTS, nullptr);
}

Epoll::~Epoll()
{

    if (_epfd != -1)
    {
        erro(close(_epfd) == -1, "close failed");
    }
    delete[] _events;
    _events = nullptr;
}

void Epoll::addFd(int fd, uint32_t op)
{
    _ev.events = op;
    _ev.data.fd = fd;
    erro(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &_ev), "add fd failed");
}

void Epoll::modFd(int fd, uint32_t op)
{
    _ev.events = op;
    _ev.data.fd = fd;
    erro(epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &_ev), "modify fd failed");
}

void Epoll::delFd(int fd, uint32_t op)
{
    _ev.events = op;
    _ev.data.fd = fd;
    erro(epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &_ev), "delete fd failed");
}

std::vector<Channel *> Epoll::wait()
{
    int nfds = epoll_wait(_epfd, _events, MAX_EVENTS, -1);
    erro(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; i++)
    {
        _vec_chs[i] = static_cast<Channel *>(_events[i].data.ptr);
        _vec_chs[i]->setRop(_events[i].events);
    }
    return std::vector<Channel *>(_vec_chs.begin(), _vec_chs.begin() + nfds);
}

void Epoll::updateChannel(Channel *ch)
{
    int fd = ch->getFd();
    _ev.events = ch->getOp();
    _ev.data.ptr = ch;
    if (!ch->getInEpoll())
    {
        erro(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &_ev), "add fd failed");
        ch->setInEpoll();
    }
    else
    {
        erro(epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &_ev), "modify fd failed");
    }
}
} // namespace WS