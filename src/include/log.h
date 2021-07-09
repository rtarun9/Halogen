#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <cstring>
#include <cstdarg>
#include <stdio.h>

#define MAX_MESSAGE_LENGTH 512

enum LogType
{
    information = 0,
    warning = 1,
    error = 2
};

/* TODO : convert functions to variadic functions in future. */
namespace halogen
{
    /* Basic logging functions to be used through out the engine. */
    inline void ASSERT(bool condition, const char *message)
    {
        if (condition)
        {
            throw std::runtime_error(message);
        }
    }

    namespace debug
    {
        inline void log(const char *message)
        {
            std::cout << "[LOG] : " << message << '\n';
        }

        inline void warning(const char *message)
        {
            std::cout << "[WARNING] : " << message << '\n';
        }

        inline void error(const char *message)
        {
            std::cerr << "[ERROR] : " << message << '\n';
            throw std::runtime_error(message);
        }
    }
}
#endif