#ifndef __HTTP_CONTEXT_H__
#define __HTTP_CONTEXT_H__

#include "../base/common.h"
#include <memory>
namespace WS
{
class HttpRequest;
class HttpContext
{
  public:
    enum HttpParseState
    {
        Invalid,

        Start,
        Method,

        // Before_URL,
        URL,

        // Before_URL_Param_Key,
        URL_Param_Key,
        // Before_URL_Param_Value,
        URL_Param_Value,

        // Before_Protocol,
        Protocol,

        // Before_Version,
        // Version_Split,
        Version,

        CR,
        LF,

        // Header,
        Header_Key,
        // Colon,
        // Before_Header_Value,
        Header_Value,

        // Before_Body,
        Body,

        Complete,
    };

  protected:
    std::unique_ptr<HttpRequest> _http_request;
    HttpParseState _http_parse_state;

  public:
    DISALLOW_COPY_AND_MOVE(HttpContext)

    HttpContext();
    ~HttpContext();

    bool parseRequest(const char* begin_, int size);
    void resetHttpContextState();
    HttpParseState getHttpParseState() const;
    HttpRequest* getHttpRequest();
};
} // namespace WS

#endif