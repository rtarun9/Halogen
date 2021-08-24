#include "../../include/core/internal/timer.h"

namespace halogen::time
{
	Clock& Clock::instance()
	{
		static Clock s_instance;
		return s_instance;
	}

	Timer::Timer(): m_current_time(0), m_end_time(0), m_start_time(0)
	{
	}

	Timer::Timer(uint32_t end_time): m_current_time(0), m_end_time(end_time), m_start_time(0)
	{
	}

	void Timer::set_timer(uint32_t end_time)
	{
		m_end_time = end_time;
	}

	void Timer::start_timer()
	{
		for (m_current_time = m_start_time; m_current_time < m_end_time; m_current_time++)
		{
			//Do nothing for now.
			;
		}

		debug::log("Timer ended (end time : ", m_end_time);
	}

	uint32_t Timer::get_current_time()
	{
		debug::log("Current time : ", m_current_time);
		return m_current_time;
	}
}