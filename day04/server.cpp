#include "server.h"
#include "acceptor.h"
#include "connection.h"
#include "event_loop.h"
#include "socket.h"
#include "src/channel.h"
#include <cstdio>

#define MAX_EVENTS 1024
#define BUFF_SIZE 1024

using namespace WS;

int main()
{
    EventLoop *loop = new EventLoop;
    Server *server = new Server(loop);
    server->onConnect([](Connection *conn) {
        conn->read();
        if (conn->getState() == Connection::State::Closed)
        {
            conn->close();
        }
        else if (conn->getState() == Connection::State::Connected)
        {
            printf("Client(%d): %s\n", conn->getSocket()->getFd(), conn->getMsg());
            conn->setMsg(conn->getMsg());
            conn->write();
        }
    });

    loop->loop();

    delete server;
    delete loop;

    return 0;
}