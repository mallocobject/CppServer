#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "../base/common.h"
#include <cstdint>
#include <functional>
#include <memory>
namespace WS
{
class TcpServer;
class TcpConnection;
class HttpRequest;
class HttpResponse;
class HttpServer
{
  protected:
    std::unique_ptr<TcpServer> _server;
    std::function<void(const HttpRequest&, HttpResponse*)> _response_callback;

  public:
    DISALLOW_DEFAULT(HttpServer)
    DISALLOW_COPY_AND_MOVE(HttpServer)

    HttpServer(const char* ip, uint16_t port);
    ~HttpServer();

    void setHttpCallback(
	const std::function<void(const HttpRequest&, HttpResponse*)>& cb);

    void startup();

    void onConnection(const std::shared_ptr<TcpConnection>& conn);
    void onMessage(const std::shared_ptr<TcpConnection>& conn);
    void onRequest(const std::shared_ptr<TcpConnection>& conn,
		   const HttpRequest& request);

    void setThreadNums(size_t num_ths);

  protected:
    void HttpDefaultCallback(const HttpRequest& request,
			     HttpResponse* response);
};
} // namespace WS

#endif