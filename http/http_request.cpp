#include "http_request.h"
#include <string>

namespace WS
{
HttpRequest::HttpRequest() : _method(Method::INVALID), _version(Unknown)
{
}

HttpRequest::~HttpRequest() = default;

void HttpRequest::setMethod(const std::string &method)
{
    if (method == "GET")
    {
        _method = Method::GET;
    }
    else if (method == "POST")
    {
        _method = Method::POST;
    }
    else if (method == "HEAD")
    {
        _method = Method::HEAD;
    }
    else if (method == "PUT")
    {
        _method = Method::PUT;
    }
    else if (method == "DELETE")
    {
        _method = Method::DELETE;
    }
    else
    {
        _method = Method::INVALID;
    }
}

HttpRequest::Method HttpRequest::getMethod() const
{
    return _method;
}

std::string HttpRequest::getMethodString() const
{
    std::string method;
    switch (_method)
    {
    case Method::GET: {
        method = "GET";
        break;
    }
    case Method::POST: {
        method = "POST";
        break;
    }
    case Method::HEAD: {
        method = "HEAD";
        break;
    }
    case Method::PUT: {
        method = "PUT";
        break;
    }
    case Method::DELETE: {
        method = "DELETE";
        break;
    }
    default: {
        method = "INVALID";
        break;
    }
    }

    return method;
}

void HttpRequest::setURL(const std::string &url)
{
    _url = url;
}

const std::string &HttpRequest::getURL() const
{
    return _url;
}

void HttpRequest::addRequestParam(const std::string &key, const std::string &value)
{
    _request_params[key] = value;
}

const std::string &HttpRequest::getRequestValue(const std::string &key) const
{
    return _request_params.at(key);
}

const std::unordered_map<std::string, std::string> &HttpRequest::getRequestParams() const
{
    return _request_params;
}

void HttpRequest::setProtocol(const std::string &protocol)
{
    _protocol = protocol;
}

const std::string &HttpRequest::getProtocol() const
{
    return _protocol;
}

void HttpRequest::setVersion(const std::string &version)
{
    if (version == "1.0")
    {
        _version = Version::Http10;
    }
    else if (version == "1.1")
    {
        _version = Version::Http11;
    }
    else
    {
        _version = Version::Unknown;
    }
}

HttpRequest::Version HttpRequest::getVersion() const
{
    return _version;
}

std::string HttpRequest::getVersionString() const
{
    std::string version;
    switch (_version)
    {
    case Version::Http10: {
        version = "http1.0";
        break;
    }
    case Version::Http11: {
        version = "http1.1";
        break;
    }
    default: {
        version = "Unknown";
        break;
    }
    }
    return version;
}

void HttpRequest::addHeader(const std::string &key, const std::string &value)
{
    _headers[key] = value;
}

const std::string &HttpRequest::getHeaderValue(const std::string &key) const
{
    return _headers.at(key);
}

const std::unordered_map<std::string, std::string> &HttpRequest::getHeaderParams() const
{
    return _headers;
}

void HttpRequest::setBody(const std::string &body)
{
    _body = body;
}

const std::string &HttpRequest::getBody() const
{
    return _body;
}

} // namespace WS