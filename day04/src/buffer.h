#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <cstddef>
#include <iostream>
#include <string>
#include <sys/types.h>
namespace WS
{
class Buffer
{
  protected:
    std::string _buf;

  public:
    void append(const char *c_str, size_t size)
    {
        _buf.append(c_str, size);
    }
    size_t size() const
    {
        return _buf.size();
    }
    const char *c_str()
    {
        return _buf.c_str();
    }
    void clear()
    {
        _buf.clear();
    }
    void getLine()
    {
        _buf.clear();
        std::getline(std::cin, _buf);
    }
};
} // namespace WS

#endif