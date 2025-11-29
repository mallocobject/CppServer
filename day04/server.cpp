#include "server.h"
#include "acceptor.h"
#include "event_loop.h"
#include "src/channel.h"
#include "src/epoll.h"
#include <cstdio>

#define MAX_EVENTS 1024
#define BUFF_SIZE 1024

using namespace WS;

int main()
{
    EventLoop *loop = new EventLoop(new Epoll);
    Server *server = new Server(loop);

    loop->loop();

    delete server;
    delete loop;

    return 0;
}