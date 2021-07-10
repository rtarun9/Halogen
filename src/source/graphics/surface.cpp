#include "../../include/core/graphics/surface.h"

namespace halogen
{
    Surface::Surface(VkInstance instance, SDL_Window& window) : m_window_surface(VK_NULL_HANDLE), m_vulkan_instance_reference(instance)
    {
        create_window_surface(instance, window);
    }

    VkSurfaceKHR Surface::get_window_surface()
    {
        return m_window_surface;
    }

    void Surface::close()
    {
        vkDestroySurfaceKHR(m_vulkan_instance_reference, m_window_surface, nullptr);
    }

    Surface::~Surface()
    {
        close();
    }

    void Surface::create_window_surface(VkInstance instance, SDL_Window& window)
    {
        if (SDL_Vulkan_CreateSurface(&window, instance , &m_window_surface) == SDL_FALSE)
        {
            debug::error("Failed to create window surface.");
        }

        debug::log("Created window surface.");
    }
}