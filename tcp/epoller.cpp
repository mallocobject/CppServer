#include "epoller.h"
#include "../base/utils.h"
#include "channel.h"
#include <strings.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

#define MAX_EVENTS 1024

namespace WS
{
Epoller::Epoller() : _events(new epoll_event[MAX_EVENTS])
{
    _epfd = epoll_create1(0);
    erro(_epfd == -1, "epoll create failed");
    _vec_chs.resize(MAX_EVENTS, nullptr);
}

Epoller::~Epoller()
{

    if (_epfd != -1)
    {
        erro(::close(_epfd) == -1, "close failed");
        _epfd = -1;
    }
    delete[] _events;
    _events = nullptr;
}

std::vector<Channel *> Epoller::poll(int timeout)
{

    int nfds = epoll_wait(_epfd, _events, MAX_EVENTS, timeout);
    erro(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; i++)
    {
        _vec_chs[i] = static_cast<Channel *>(_events[i].data.ptr);
        _vec_chs[i]->setReadyEvents(_events[i].events);
    }
    return std::vector<Channel *>(_vec_chs.begin(), _vec_chs.begin() + nfds);
}

void Epoller::updateChannel(Channel *ch)
{
    int fd = ch->getFd();
    epoll_event ev;
    ev.events = ch->getListenEvents();
    ev.data.ptr = ch;
    if (!ch->inEpoller())
    {
        erro(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "add fd failed");
        ch->setInEpoller(true);
    }
    else
    {
        erro(epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "modify fd failed");
    }
}

void Epoller::deleteChannel(Channel *ch)
{
    if (ch->inEpoller())
    {
        erro(epoll_ctl(_epfd, EPOLL_CTL_DEL, ch->getFd(), nullptr) == -1, "delete fd failed");
        ch->setInEpoller(false);
    }
}

} // namespace WS