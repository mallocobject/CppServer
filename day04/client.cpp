#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    // 创建 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket failed");
        return 1;
    }

    // 设置服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    std::cout << "Connecting to 127.0.0.1:8888..." << std::endl;

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect failed");
        close(sockfd);
        return 1;
    }

    std::cout << "Connected successfully!" << std::endl;

    // 发送测试数据
    std::string message = "Hello, Server!";
    std::cout << "Sending: " << message << std::endl;

    ssize_t sent = send(sockfd, message.c_str(), message.length(), 0);
    if (sent == -1)
    {
        perror("send failed");
        close(sockfd);
        return 1;
    }

    std::cout << "Sent " << sent << " bytes" << std::endl;

    // 接收响应
    char buffer[1024];
    ssize_t received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

    if (received > 0)
    {
        buffer[received] = '\0';
        std::cout << "Received: " << buffer << std::endl;
        std::cout << "Received " << received << " bytes" << std::endl;
    }
    else if (received == 0)
    {
        std::cout << "Server closed connection" << std::endl;
    }
    else
    {
        perror("recv failed");
    }

    // // 关闭连接
    // close(sockfd);
    // std::cout << "Connection closed" << std::endl;

    while (true)
    {
    }
    return 0;
}