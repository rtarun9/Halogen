#ifndef PLATFORM_H
#define PLATFORM_H

#include "../../include/log.h"
#include "../configuration.h"
#include "../../include/core/window.h"

#include <memory>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <cstring>

namespace halogen
{
    /* Contains basic platform - dependant functions. */
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