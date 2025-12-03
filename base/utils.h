#ifndef __UTILS_H__
#define __UTILS_H__

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace WS
{
#define erro(condition, message)                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (condition)                                                                             \
        {                                                                                          \
            const int saved_errno = errno;                                                         \
            std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] ERROR: " << message << " - "     \
                      << std::strerror(saved_errno) << std::endl;                                  \
            std::exit(EXIT_FAILURE);                                                               \
        }                                                                                          \
    } while (0)
} // namespace WS

#endif