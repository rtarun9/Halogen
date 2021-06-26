#include "../include/core/platform.h"

namespace halogen
{
    Platform::Platform()
    {
        m_platform_backend = configuration::PLATFORM_BACKEND;
    }

    void Platform::initialize_backend()
    {
        if (m_platform_backend == PlatformBackend::SDL2)
        {
            if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
            {
                debug::error("Could not initialize SDL.");
            }
            debug::log("Successfully initialized SDL.");

        }
    }

    void Platform::close_backend()
    {
        debug::log("Closing platform");
        if (m_platform_backend == PlatformBackend::SDL2)
        {
            SDL_Quit();
        }
    }

    Platform::~Platform()
    {
        close_backend();
    }

}
