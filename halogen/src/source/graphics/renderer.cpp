#include "../../include/core/graphics/renderer.h"

namespace halogen
{
    Renderer::Renderer()
    {
        m_vulkan_instance = nullptr;
    }

    void Renderer::initialize_renderer(std::shared_ptr<Window> window)
    {
        unsigned int instance_level_extension_count;
        SDL_Vulkan_GetInstanceExtensions(&window->get_window(), &instance_level_extension_count, nullptr);

        VkApplicationInfo application_info = {};
        application_info.pApplicationName = "Name";
        application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        application_info.pEngineName = "No Engine";
        application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        application_info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instance_create_info = {};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &application_info;
        instance_create_info.enabledLayerCount = 0;
        instance_create_info.ppEnabledLayerNames = nullptr;
        instance_create_info.enabledExtensionCount = 0;
        instance_create_info.ppEnabledExtensionNames = nullptr;

        if (vkCreateInstance(&instance_create_info, nullptr, &m_vulkan_instance) != VK_SUCCESS)
        {
            debug::error("Could not create vulkan_instance");
        }

        debug::log("Successfully initialized vulkan renderer");
    }
}