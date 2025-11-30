#include "buffer.h"
#include "connection.h"
#include "src/inet_addr.h"
#include "src/socket.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <strings.h>
#include <sys/types.h>

#define MAX_EVENTS 1024
#define BUFF_SIZE 1024

using namespace WS;

int main()
{

    Socket *sock = new Socket;
    InetAddr addr("127.0.0.1", 8888);

    sock->connect(&addr);

    Connection conn(nullptr, sock, false);

    while (true)
    {
        printf("client: ");
        conn.typeLineSendBuffer();
        conn.write();
        if (conn.getState() == Connection::State::Closed)
        {
            conn.close();
            break;
        }
        conn.read();
        if (conn.getState() == Connection::State::Closed)
        {
            conn.close();
            break;
        }
        printf("Server: %s\n", conn.getMsg());
    }

    return 0;
}