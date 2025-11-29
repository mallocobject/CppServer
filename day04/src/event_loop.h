#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

namespace WS
{

class Epoll;
class Channel;
class EventLoop
{
  protected:
    Epoll *_ep;
    bool _quit;

  public:
    EventLoop();
    ~EventLoop();
    void loop();
    void updateChannel(Channel *ch);
};
} // namespace WS

#endif