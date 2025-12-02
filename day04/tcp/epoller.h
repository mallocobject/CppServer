#ifndef __EPOLLER_H__
#define __EPOLLER_H__

#include "../base/common.h"
#include <fcntl.h>
#include <sys/epoll.h>
#include <vector>
namespace WS
{
class Channel;
class Epoller
{
  protected:
    int _epfd;
    epoll_event *_events;
    std::vector<Channel *> _vec_chs;

  public:
    DISALLOW_COPY_AND_MOVE(Epoller)

    Epoller();
    ~Epoller();

    std::vector<Channel *> poll(int timeout = -1);

    void updateChannel(Channel *ch);
    void deleteChannel(Channel *ch);
};

} // namespace WS

#endif