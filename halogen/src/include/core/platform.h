#ifndef PLATFORM_H
#define PLATFORM_H

#include "../../include/log.h"
#include "../../include/core/configuration.h"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <cstring>

namespace halogen
{
    class Platform
    {
    public:
        Platform();

        void initialize_backend();

        void close_backend();

        ~Platform();

    private:
        PlatformBackend m_platform_backend;
    };
}
#endif