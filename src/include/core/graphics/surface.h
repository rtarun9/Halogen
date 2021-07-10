#ifndef SURFACE_H
#define SURFACE_H

#include "../../configuration.h"
#include "instance.h"
#include "../window.h"

#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>
#include <memory>

namespace halogen
{
    class Surface
    {
    public:
        Surface(VkInstance instance, SDL_Window& window);

        [[nodiscard]]
        VkSurfaceKHR get_window_surface();

        void close();
        ~Surface();

    private:
        void create_window_surface(VkInstance instance, SDL_Window& window);

    private:
        VkSurfaceKHR m_window_surface;

        /* Reference to instance since it will be required by the instance. */
        VkInstance m_vulkan_instance_reference;
    };
}
#endif