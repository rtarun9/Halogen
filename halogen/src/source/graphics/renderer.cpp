#include "../../include/core/graphics/renderer.h"

namespace halogen
{
    Renderer::Renderer(std::shared_ptr<Window>& window) : m_window(window)
    {
        m_vulkan_instance = nullptr;
    }

    void Renderer::initialize_renderer()
    {
        initialize_vulkan();
    }

    void Renderer::initialize_vulkan()
    {
        create_vulkan_instance();
    }

    void Renderer::create_vulkan_instance()
    {
        if (configuration::DEBUG == true && (check_validation_layer_support() == false))
        {
            debug::error("Failed to find all validation layers");
        }

       vk::ApplicationInfo application_info = vk::ApplicationInfo()
               .setPNext(nullptr)
               .setPApplicationName("halogen")
               .setApplicationVersion(1)
               .setPEngineName("halogen")
               .setEngineVersion(1)
               .setApiVersion(VK_API_VERSION_1_1);

       std::vector<const char *> extension_names;
       uint32_t extension_count = 0;

        SDL_Vulkan_GetInstanceExtensions(&m_window->get_window(), &extension_count, nullptr);

        extension_names.resize(extension_count);
        SDL_Vulkan_GetInstanceExtensions(&m_window->get_window(), &extension_count, extension_names.data());

        vk::InstanceCreateInfo instance_create_info = vk::InstanceCreateInfo()
               .setPNext(nullptr)
               .setPApplicationInfo(&application_info)
               .setEnabledExtensionCount(extension_count)
               .setPEnabledExtensionNames(extension_names)
               .setEnabledLayerCount(0)
               .setPpEnabledLayerNames(nullptr);

       vk::Result instance_creation_result = vk::createInstance(&instance_create_info, nullptr, &m_vulkan_instance);
       if (instance_creation_result == vk::Result::eErrorExtensionNotPresent)
       {
           debug::error("Requested extensions not present");
       }
       else if (instance_creation_result == vk::Result::eErrorLayerNotPresent)
       {
           debug::error("Requested validation layers not present.");
       }
    }

    void Renderer::setup_debug_messenger()
    {
        if (configuration::DEBUG == false)
        {
            return;
        }

        vk::DebugUtilsMessengerCreateInfoEXT messenger_create_info = vk::DebugUtilsMessengerCreateInfoEXT()
                .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
                .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
                .setPUserData(nullptr)
                .setPfnUserCallback(nullptr);
    }
    void Renderer::cleanup()
    {
        m_vulkan_instance.destroy();
    }

    bool Renderer::check_validation_layer_support()
    {
        uint32_t validation_layer_count = 0;
        vk::enumerateInstanceLayerProperties(&validation_layer_count, nullptr);

        std::vector<vk::LayerProperties> available_validation_layers(validation_layer_count);
        vk::enumerateInstanceLayerProperties(&validation_layer_count, available_validation_layers.data());

        for (const char *requested_layer : configuration::VALIDATION_LAYERS)
        {
            bool layer_present = false;
            for (vk::LayerProperties layer : available_validation_layers)
            {
                if (strcmp(requested_layer, layer.layerName) == 0)
                {
                    layer_present = true;
                }
            }

            if (layer_present == false)
            {
                debug::error("Failed to find requested validation layers.");
                return false;
            }
        }

        return true;
    }
}