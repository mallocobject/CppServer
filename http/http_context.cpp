#include "http_context.h"
#include "http_request.h"
#include <cctype>
#include <memory>
#include <string>

namespace WS
{
HttpContext::HttpContext() : _http_parse_state(HttpParseState::Start)
{
    _http_request = std::make_unique<HttpRequest>();
}

HttpContext::~HttpContext() = default;

void HttpContext::resetHttpContextState()
{
    _http_parse_state = HttpParseState::Start;
}
HttpRequest *HttpContext::getHttpRequest()
{
    return _http_request.get();
}

HttpContext::HttpParseState HttpContext::getHttpParseState() const
{
    return _http_parse_state;
}

bool HttpContext::parseRequest(const char *begin_, int size)
{
    char *begin = const_cast<char *>(begin_);
    char *end = begin;
    std::string key;
    std::string value;
    while (_http_parse_state != HttpParseState::Invalid &&
           _http_parse_state != HttpParseState::Complete && end - begin <= size)
    {
        char ch = *end;
        switch (_http_parse_state)
        {
        case HttpParseState::Start: {
            if (ch == '\r' || ch == '\n' || isblank(ch))
            {
            }
            else if (isupper(ch))
            {
                begin = end;
                _http_parse_state = HttpParseState::Method;
            }
            else
            {
                _http_parse_state = HttpParseState::Invalid;
            }
            break;
        }
        case HttpParseState::Method: {
            if (isblank(ch))
            {
                _http_request->setMethod(std::string(begin, end));
                begin = end + 1;
                _http_parse_state = HttpParseState::URL;
            }
            break;
        }
        case HttpParseState::URL: {
            if (isblank(ch))
            {
                _http_request->setURL(std::string(begin, end));
                begin = end + 1;
                _http_parse_state = HttpParseState::Protocol;
            }
            else if (ch == '?')
            {
                _http_request->setURL(std::string(begin, end));
                begin = end + 1;
                _http_parse_state = HttpParseState::URL_Param_Key;
            }
            break;
        }
        case HttpParseState::URL_Param_Key: {
            if (ch == '=')
            {
                key = std::string(begin, end);
                begin = end + 1;
                _http_parse_state = HttpParseState::URL_Param_Value;
            }
            break;
        }
        case HttpParseState::URL_Param_Value: {
            if (isblank(ch))
            {
                value = std::string(begin, end);
                _http_request->addRequestParam(key, value);
                begin = end + 1;
                _http_parse_state = HttpParseState::Protocol;
            }
            else if (ch == '&')
            {
                value = std::string(begin, end);
                _http_request->addRequestParam(key, value);
                begin = end + 1;
                _http_parse_state = HttpParseState::URL_Param_Key;
            }
            break;
        }
        case HttpParseState::Protocol: {
            if (ch == '/')
            {
                _http_request->setProtocol(std::string(begin, end));
                begin = end + 1;
                _http_parse_state = HttpParseState::Version;
            }
            break;
        }
        case HttpParseState::Version: {
            if (ch == '\r')
            {
                _http_request->setVersion(std::string(begin, end));
                _http_parse_state = HttpParseState::CR;
            }
            break;
        }
        case HttpParseState::CR: {
            if (ch == '\n')
            {
                _http_parse_state = HttpParseState::LF;
                // begin = end + 1;
            }
            break;
        }
        case HttpParseState::LF: {
            if (ch == '\r')
            {
                begin = end + 2;
                _http_parse_state = HttpParseState::Body;
            }
            else
            {
                begin = end;
                _http_parse_state = HttpParseState::Header_Key;
            }
            break;
        }
        case HttpParseState::Header_Key: {
            if (ch == ':')
            {
                key = std::string(begin, end);
                begin = end + 2;
                _http_parse_state = HttpParseState::Header_Value;
            }
            break;
        }
        case HttpParseState::Header_Value: {
            if (ch == '\r')
            {
                value = std::string(begin, end);
                _http_request->addHeader(key, value);
                // begin = end + 1;
                _http_parse_state = HttpParseState::CR;
            }
            break;
        }
        case HttpParseState::Body: {
            int body_length = size - (end - begin_);
            _http_request->setBody(std::string(begin, begin + body_length));
            _http_parse_state = HttpParseState::Complete;
            break;
        }

        default: {
            _http_parse_state = HttpParseState::Invalid;
            break;
        }
        }

        end++;
    }
    return _http_parse_state == HttpParseState::Complete;
}

} // namespace WS