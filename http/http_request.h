#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include "../base/common.h"
#include <string>
#include <unordered_map>
namespace WS
{
class HttpRequest
{
  public:
    enum Method
    {
        INVALID = 0,
        GET,
        POST,
        HEAD,
        PUT,
        DELETE,
    };

    enum Version
    {
        Unknown = 0,
        Http10,
        Http11,
    };

  protected:
    Method _method;
    std::string _url;
    std::unordered_map<std::string, std::string> _request_params;
    std::string _protocol; // ?
    Version _version;      // http/xx
    std::unordered_map<std::string, std::string> _headers;
    std::string _body;

  public:
    DISALLOW_COPY_AND_MOVE(HttpRequest)

    HttpRequest();
    ~HttpRequest();

    void setMethod(const std::string &method);
    Method getMethod() const;
    std::string getMethodString() const;

    void setURL(const std::string &url);
    const std::string &getURL() const;

    void addRequestParam(const std::string &key, const std::string &value);
    const std::string &getRequestValue(const std::string &key) const;
    const std::unordered_map<std::string, std::string> &getRequestParams() const;

    void setProtocol(const std::string &protocol);
    const std::string &getProtocol() const;

    void setVersion(const std::string &version);
    Version getVersion() const;
    std::string getVersionString() const;

    void addHeader(const std::string &key, const std::string &value);
    const std::string &getHeaderValue(const std::string &key) const;
    const std::unordered_map<std::string, std::string> &getHeaderParams() const;

    void setBody(const std::string &body);
    const std::string &getBody() const;
};
} // namespace WS

#endif