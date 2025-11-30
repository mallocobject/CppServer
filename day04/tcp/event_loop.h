#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include "common.h"
#include <memory>
namespace WS
{

class Epoll;
class Channel;
class EventLoop
{
  protected:
    std::unique_ptr<Epoll> _ep;

  public:
    DISALLOW_COPY_AND_MOVE(EventLoop)

    EventLoop();
    ~EventLoop();
    void loop();
    void updateChannel(Channel *ch);
    void deleteChannel(Channel *ch);
};
} // namespace WS

#endif