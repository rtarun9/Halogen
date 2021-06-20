#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <cstring>

namespace halogen
{
    namespace assert
    {
        inline void error(const char *error_message)
        {
            throw std::runtime_error(error_message);
        }

        inline void error_condition(bool condition, const char *condition_fail_message)
        {
            if (!condition)
            {
              error(condition_fail_message);
            }
        }
    }
}
#endif