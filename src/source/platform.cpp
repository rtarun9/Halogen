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

    void Platform::get_instance_extensions(std::vector<const char *> &instance_extensions, uint32_t *instance_extension_count)
    {
        SDL_Vulkan_GetInstanceExtensions(nullptr, instance_extension_count, nullptr);
        instance_extensions.reserve(*instance_extension_count);

        SDL_Vulkan_GetInstanceExtensions(nullptr, instance_extension_count, instance_extensions.data());

        if (configuration::DEBUG)
        {
            instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            *(instance_extension_count) += 1;
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
