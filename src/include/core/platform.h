#ifndef PLATFORM_H
#define PLATFORM_H

#include "../log.h"
#include "../configuration.h"
#include "window.h"

#include <vulkan/vulkan.h>
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

        static void get_instance_extensions(std::vector<const char *>& instance_extensions, uint32_t *instance_extension_count);

        void close_backend();

        ~Platform();

    private:
        PlatformBackend m_platform_backend;
    };
}
#endif