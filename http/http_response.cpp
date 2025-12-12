#include "http_response.h"
#include <algorithm>
#include <string>

namespace WS
{
HttpResponse::HttpResponse(bool close_connection)
    : _close_connection(close_connection), _state_code(HttpStateCode::Unknown)
{
}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatusCode(HttpResponse::HttpStateCode state_code)
{
    _state_code = state_code;
}

void HttpResponse::setStatusMessage(const std::string& status_message)
{
    _status_message = std::move(status_message);
}

void HttpResponse::setCloseConnection(bool close_connection)
{
    _close_connection = close_connection;
}

void HttpResponse::setContentType(const std::string& content_type)
{
    addHeader("Content-Type", content_type);
}

void HttpResponse::addHeader(const std::string& key, const std::string& value)
{
    _headers[key] = value;
}

void HttpResponse::setBody(const std::string& body) { _body = std::move(body); }

bool HttpResponse::IsCloseConnection() const { return _close_connection; }

std::string HttpResponse::message()
{
    std::string message;
    message += "HTTP/1.1 " + std::to_string(_state_code) + " " +
               _status_message + "\r\n";
    if (_close_connection)
    {
        message += "Connection: close\r\n";
    }
    else
    {
        message += "Content-Length: " + std::to_string(_body.size()) + "\r\n";
        message += "Connection: keep-alive\r\n";
    }

    for (const auto& header : _headers)
    {
        message += header.first + ": " + header.second + "\r\n";
    }

    message += "\r\n";
    message += _body;

    return message;
}
} // namespace WS