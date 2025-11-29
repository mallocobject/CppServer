#include "buffer.h"
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

    Socket sock;
    InetAddr addr("127.0.0.1", 8888);

    sock.connect(&addr);

    char buf[BUFF_SIZE];

    Buffer send_buf;
    Buffer recv_buf;

    while (true)
    {
        printf("client: ");
        send_buf.getLine();

        sock.write(send_buf.c_str(), send_buf.size());

        int already_read = 0;

        while (true)
        {
            ssize_t bytes_read = sock.read(buf, sizeof(buf));
            if (bytes_read > 0)
            {
                recv_buf.append(buf, bytes_read);
                already_read += bytes_read;
            }
            else if (bytes_read == 0)
            {
                printf("server closed connection\n");
                exit(EXIT_SUCCESS);
            }
            if (already_read >= send_buf.size())
            {
                printf("server: %s\n", recv_buf.c_str());
                break;
            }
        }
        recv_buf.clear();
    }
}