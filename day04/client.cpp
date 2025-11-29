#include "src/inet_addr.h"
#include "src/socket.h"
#include <cstdio>
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
    InetAddr *addr = new InetAddr("127.0.0.1", 8888);

    sock->connect(addr);

    char buf[BUFF_SIZE + 1];
    size_t nbytes = BUFF_SIZE;

    while (true)
    {
        bzero(buf, nbytes);

        printf("client: ");
        scanf("%s", buf);

        sock->write(buf, strlen(buf));

        ssize_t bytes_read = sock->read(buf, nbytes);
        if (bytes_read > 0)
        {
            buf[bytes_read] = '\0';
            printf("server: %s\n", buf);
        }
        else if (bytes_read == 0)
        {
            printf("server closed connection\n");
            return 0;
        }
        else
        {
            perror("read");
            return -1;
        }
    }

    delete addr;
    delete sock;
}