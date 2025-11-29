#ifndef __EPOLL_H__
#define __EPOLL_H__

#include <fcntl.h>
#include <sys/epoll.h>
#include <vector>
namespace WS
{
class Channel;
class Epoll
{
  protected:
    int _epfd;
    epoll_event _ev;
    epoll_event *_events;
    std::vector<Channel *> _vec_chs;

  public:
    Epoll();
    ~Epoll();

    void addFd(int fd, uint32_t op);

    void modFd(int fd, uint32_t op);

    void delFd(int fd, uint32_t op);

    std::vector<Channel *> wait();

    void updateChannel(Channel *ch);

  protected:
};

} // namespace WS

#endif