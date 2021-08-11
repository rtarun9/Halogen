#pragma once

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <stdexcept>

namespace halogen
{
    //Call when some impossible conditions is faced.
    inline void ASSERT(bool condition, const char *message)
    {
        if (condition)
        {
            std::cout << "[ASSERT] :  " << message << '\n';
            exit(EXIT_FAILURE);
        }
    }

    namespace debug
    {
        //Base case for recursive call (0 arguments).
        static void print()
        {
            std::cout<< '\n';
        }

        //Template functions for print (recursive)
        template <typename First, typename ...Rest>
        inline void print(const First& first, const Rest& ...rest)
        {
            std::cout << first << ' ';
            print(rest...);
        }

        //Basic logging functions to be used through out the engine.

        template <typename First, typename ...Rest>
        inline void log(const First& first, const Rest& ...rest)
        {
            std::cout << "[LOG] : ";
            print(first, rest...);
        }

        template <typename First, typename ...Rest>
        inline void warning(const First& first, const Rest& ...rest)
        {
            std::cout << "[Warning] : ";
            std::cout << "Line : " << __LINE__ << ' ';
            print(first, rest...);
        }

        template <typename First, typename ...Rest>
        inline void error(const First& first, const Rest& ...rest)
        {
            std::cout << "[ERROR] : ";
            print(first, rest...);
            exit(EXIT_FAILURE);
        }
    }
}

