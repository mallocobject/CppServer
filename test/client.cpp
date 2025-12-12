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
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
	-1)
    {
	perror("connect failed");
	close(sockfd);
	return 1;
    }

    std::cout << "Connected successfully!" << std::endl;

    // 发送测试数据
    std::string message =
	"GET /hello?a=2 HTTP/1.1\r\n"
	"Host: 127.0.0.1:1234\r\n"
	"Connection: keep-alive\r\n"
	"Cache-Control: max-age=0\r\n"
	"sec-ch-ua: \"Google Chrome\";v=\"113\", \"Chromium\";v=\"113\", "
	"\"Not-A.Brand\";v=\"24\"\r\n"
	"sec-ch-ua-mobile: ?0\r\n"
	"sec-ch-ua-platform: \"Linux\"\r\n"
	"Upgrade-Insecure-Requests: 1\r\n"
	"User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
	"(KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36\r\n"
	"Accept: "
	"text/html,application/xhtml+xml,application/xml;q=0.9,image/"
	"avif,image/webp,image/apng,*/*;q=0.8,application/"
	"signed-exchange;v=b3;q=0.7\r\n"
	"Sec-Fetch-Site: none\r\n"
	"Sec-Fetch-Mode: navigate\r\n"
	"Sec-Fetch-User: ?1\r\n"
	"Sec-Fetch-Dest: document\r\n"
	"Accept-Encoding: gzip, deflate, br\r\n"
	"Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,zh-TW;q=0.7\r\n"
	"Cookie: "
	"username-127-0-0-1-8888=\"2|1:0|10:1681994652|23:username-127-0-0-1-"
	"8888|44:Yzg5ZjA1OGU0MWQ1NGNlMWI2MGQwYTFhMDAxYzY3YzU=|"
	"6d0b051e144fa862c61464acf2d14418d9ba26107549656a86d92e079ff033ea\"; "
	"_xsrf=2|dd035ca7|e419a1d40c38998f604fb6748dc79a10|168199465\r\n"
	"\r\n";
    // std::cout << "Sending: " << message << std::endl;

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

    // 关闭连接
    close(sockfd);
    std::cout << "Connection closed" << std::endl;

    // while (true)
    // {
    // }
    return 0;
}