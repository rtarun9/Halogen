#pragma once

#include <chrono>

namespace halogen
{
    //Singleton class for everything time related (Time constants and run / delta time)
    //Not a very good design choice, but fine for now.
    class Timer
    {
    public:
        Timer() = default;

        Timer (const Timer& other) = delete;
        //Timer operator=(const Timer other) = delete;

        static Timer& instance();

    public:
        std::chrono::high_resolution_clock::time_point m_frame_start;
        std::chrono::high_resolution_clock::time_point m_frame_end;
        std::chrono::duration<double> m_frame_time;

    private:
        static Timer *s_instance;
    };
}