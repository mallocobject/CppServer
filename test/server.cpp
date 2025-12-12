#include "../http/http_request.h"
#include "../http/http_response.h"
#include "../http/http_server.h"
#include <memory>
#include <string>

using namespace WS;

const std::string html = " <font color=\"red\">This is html!</font> ";
void HttpResponseCallback(const HttpRequest& request, HttpResponse* response)
{
    if (request.getMethod() != HttpRequest::Method::GET)
    {
	response->setStatusCode(HttpResponse::HttpStateCode::BadRequest_400);
	response->setStatusMessage("Bad Request");
	response->setCloseConnection(true);
    }

    else
    {
	std::string url = request.getURL();
	if (url == "/")
	{
	    response->setStatusCode(HttpResponse::HttpStateCode::OK_200);
	    response->setBody(html);
	    response->setContentType("text/html");
	}
	else if (url == "/hello")
	{
	    response->setStatusCode(HttpResponse::HttpStateCode::OK_200);
	    response->setBody("hello world\n");
	    response->setContentType("text/plain");
	}
	else if (url == "/favicon.ico")
	{
	    response->setStatusCode(HttpResponse::HttpStateCode::OK_200);
	}
	else
	{
	    response->setStatusCode(HttpResponse::HttpStateCode::NotFound_404);
	    response->setStatusMessage("Not Found");
	    response->setBody("Sorry Not Found\n");
	    response->setCloseConnection(true);
	}
    }
    return;
}

int main(int argc, char* argv[])
{
    int port;
    if (argc <= 1)
    {
	port = 8888;
    }
    else if (argc == 2)
    {
	port = atoi(argv[1]);
    }
    else
    {
	printf("error");
	exit(0);
    }

    std::unique_ptr<HttpServer> server =
	std::make_unique<HttpServer>("127.0.0.1", port);
    server->setHttpCallback(HttpResponseCallback);

    server->startup();

    return 0;
}