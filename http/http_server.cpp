#include "http_server.h"
#include "../tcp/tcp_connection.h"
#include "../tcp/tcp_server.h"
#include "http_context.h"
#include "http_request.h"
#include "http_response.h"
#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>

namespace WS
{

void HttpServer::HttpDefaultCallback(const HttpRequest& request,
				     HttpResponse* response)
{
    response->setStatusCode(HttpResponse::HttpStateCode::BadRequest_400);
    response->setStatusMessage("Not Found");
    response->setCloseConnection(true);
}

HttpServer::HttpServer(const char* ip, uint16_t port)
{
    _server = std::make_unique<TcpServer>(ip, port);
    _server->setConnectionCallback(
	std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    _server->setMessageCallback(
	std::bind(&HttpServer::onMessage, this, std::placeholders::_1));
    setHttpCallback(std::bind(&HttpServer::HttpDefaultCallback, this,
			      std::placeholders::_1, std::placeholders::_2));
}

HttpServer::~HttpServer() {}

void HttpServer::onConnection(const std::shared_ptr<TcpConnection>& conn)
{
    int clnt_fd = conn->getFd();
    sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);
    getpeername(clnt_fd, (sockaddr*)&peer_addr, &peer_addr_len);

    std::cout << std::this_thread::get_id() << " new connection" << "[fd#"
	      << clnt_fd << "]" << " from " << ::inet_ntoa(peer_addr.sin_addr)
	      << ":" << ntohs(peer_addr.sin_port) << std::endl;
}

void HttpServer::onMessage(const std::shared_ptr<TcpConnection>& conn)
{
    if (conn->State() == TcpConnection::State::Connected)
    {
	HttpContext* context = conn->getHttpContext();
	if (!context->parseRequest(conn->getMsg(), conn->getMsgLen()))
	{
	    conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
	    conn->handleClose();
	}

	if (context->getHttpParseState() ==
	    HttpContext::HttpParseState::Complete)
	{
	    onRequest(conn, *context->getHttpRequest());
	    context->resetHttpContextState();
	}
    }
}

void HttpServer::setHttpCallback(
    const std::function<void(const HttpRequest&, HttpResponse*)>& cb)
{
    _response_callback = std::move(cb);
}

void HttpServer::onRequest(const std::shared_ptr<TcpConnection>& conn,
			   const HttpRequest& request)
{
    std::string connection_state = request.getHeaderValue("Connection");
    bool close = connection_state == "close" ||
		 request.getVersion() != HttpRequest::Version::Http11 ||
		 connection_state != "keep-alive";
    HttpResponse response(close);
    _response_callback(request, &response);
    conn->send(response.message().c_str());

    if (response.IsCloseConnection())
    {
	conn->handleClose();
    }
}

void HttpServer::startup() { _server->startup(); }

} // namespace WS