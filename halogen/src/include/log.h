#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <cstring>

#define MAX_MESSAGE_LENGTH 512

enum LogType
{
    information = 0,
    warning = 1,
    error = 2
};

namespace halogen
{
    inline void ASSERT(bool condition, const char *message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    namespace debug
    {
        template <typename T>
        inline void log(T message)
        {
            std::cout << "[LOG] :  " << message << '\n';
        }

        inline void warning(const char *message)
        {
            std::cout << "[WARNING] : " << message << '\n';
        }

        inline void error(const char *message)
        {
            std::cout << "[ERROR] : " << message << '\n';
            throw std::runtime_error(message);
        }
    }
}
#endif