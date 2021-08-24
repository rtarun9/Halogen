#pragma once

#include <chrono>

#include "../log.h"

namespace halogen::time
{
	//Constant time related values
	namespace
	{
		constexpr uint32_t One_Second = 1000000000;
		constexpr uint32_t One_MilliSecond = 100000000;
	}

    //Meyers singleton pattern Clock class
    class Clock
    {
    public:
        Clock(const Clock& other) = delete;
		Clock& operator=(const Clock& other) = delete;

        static Clock& instance();

      public:
        std::chrono::high_resolution_clock::time_point m_frame_start;
        std::chrono::high_resolution_clock::time_point m_frame_end;
        std::chrono::duration<double> m_frame_time;

    private:
      Clock() = default;
      ~Clock() = default;
    };

    //Timer class for creating basic stop watches
    //Note : Will be slow, need to change approach later on.
    class Timer
	{
	public:
		Timer();
		Timer(uint32_t end_time);
		~Timer() = default;

		void set_timer(uint32_t end_time);
		void start_timer();
		uint32_t get_current_time();

	private:
		uint32_t m_start_time;
		uint32_t m_end_time;
		uint32_t m_current_time;
	};
}