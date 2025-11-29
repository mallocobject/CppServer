#include "src/buffer.h"
#include "src/inet_addr.h"
#include "src/socket.h"
#include "src/thread_pool.h"
#include <functional>
#include <unistd.h>

#define MAX_EVENTS 1024
#define BUFF_SIZE 1024

using namespace std;
using namespace WS;

void oneClient(int msgs, int wait)
{
    Socket sock;
    InetAddr addr("127.0.0.1", 8888);

    sock.connect(&addr);

    Buffer send_buf;
    Buffer recv_buf;

    char buf[BUFF_SIZE];

    sleep(wait);

    int count = 0;

    while (count < msgs)
    {
        send_buf.setBuf("I'm client!");

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
                printf("count: %d, server: %s\n", count++, recv_buf.c_str());
                break;
            }
        }
        recv_buf.clear();
    }
}

int main(int argc, char *argv[])
{
    int threads = 100;
    int msgs = 100;
    int wait = 0;
    int o;
    const char *optstring = "t:m:w:";
    while ((o = getopt(argc, argv, optstring)) != -1)
    {
        switch (o)
        {
        case 't':
            threads = stoi(optarg);
            break;
        case 'm':
            msgs = stoi(optarg);
            break;
        case 'w':
            wait = stoi(optarg);
            break;
        case '?':
            printf("error optopt: %c\n", optopt);
            printf("error opterr: %d\n", opterr);
            break;
        }
    }

    ThreadPool *pool = new ThreadPool(threads);
    std::function<void()> func = std::bind(oneClient, msgs, wait);
    for (int i = 0; i < threads; ++i)
    {
        pool->addTask(func);
    }
    delete pool;
    return 0;
}