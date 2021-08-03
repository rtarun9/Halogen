#include "../include/timer.h"

halogen::Timer* halogen::Timer::s_instance = nullptr;

namespace halogen
{
    Timer& Timer::instance()
    {
        if (s_instance == nullptr)
        {
            s_instance = new Timer();
        }

        return *s_instance;
    }
}