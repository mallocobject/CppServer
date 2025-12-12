#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include "unordered_map"
#include <../base/common.h>
#include <string>
namespace WS
{
class HttpResponse
{
  public:
    enum HttpStateCode
    {
        Unknown = 1,
        Continue_100 = 100,
        OK_200 = 200,
        BadRequest_400 = 400,
        Forbidden_403 = 403,
        NotFound_404 = 404,
        InternalServerError_500 = 500,
    };

  protected:
    std::unordered_map<std::string, std::string> _headers;
    HttpStateCode _state_code;
    std::string _status_message;
    std::string _body;
    bool _close_connection;

  public:
    DISALLOW_DEFAULT(HttpResponse)
    DISALLOW_COPY_AND_MOVE(HttpResponse)

    HttpResponse(bool close_connection);
    ~HttpResponse();

    void setStatusCode(HttpStateCode state_code);
    void setStatusMessage(const std::string& status_message);
    void setCloseConnection(bool close_connection);
    void setContentType(const std::string& content_type);
    void addHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);
    std::string message();
    bool IsCloseConnection() const;
};
} // namespace WS

#endif