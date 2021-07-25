#ifndef PLATFORM_H
#define PLATFORM_H

#include "../log.h"
#include "../configuration.h"

#include <vulkan/vulkan.h>
#include <memory>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <cstring>
#include <vector>

namespace halogen
{
    /* Contains basic platform - dependant functions. */
    class Platform
    {
    public:
        Platform();
        ~Platform();

        Platform(const Platform& other) = delete;
        Platform& operator=(const Platform& other) = delete;

        static void get_instance_extensions(std::vector<const char *>& instance_extensions, uint32_t& instance_extension_count);
        static void create_window_surface(const SDL_Window& window, VkInstance instance, VkSurfaceKHR& surface);

    private:
        void initialize_backend();
        void close_backend();

    private:
        PlatformBackend m_platform_backend;
    };
}
#endif