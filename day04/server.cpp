#include "tcp_connection.h"
#include "tcp_server.h"
#include <cstdio>

#define MAX_EVENTS 1024
#define BUFF_SIZE 1024

using namespace WS;

int main()
{
    TcpServer *tcp_server = new TcpServer("127.0.0.1", 8888);
    tcp_server->setMessageCallback([](TcpConnection *conn) {
        if (conn->State() == TcpConnection::State::Connected)
        {
            printf("Client(%d): %s\n", conn->getFd(), conn->getMsg());
            conn->send(conn->getMsg());
        }
    });

    tcp_server->start();

    delete tcp_server;

    return 0;
}