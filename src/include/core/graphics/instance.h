#ifndef INSTANCE_H
#define INSTANCE_H

#include "vk_debug.h"
#include "../../log.h"
#include "../../configuration.h"
#include "vk_external.h"
#include "../platform.h"

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include <SDL_vulkan.h>

namespace halogen
{
    /* Base class for vulkan instance, and related helper instance / extension checks. Will also contain details of the debug utils messenger. */
    class Instance
    {
    public:
        Instance();
        Instance(const std::string& application_name, const std::string& engine_name);

        [[nodiscard]]
        VkInstance get_vulkan_instance();

        void close();
        ~Instance();

    private:
        void create_instance();
        void create_debug_messenger();

        /* Helper functions to get and check required instance / extensions. */
        [[nodiscard]]
        static bool check_validation_layer_support();

        [[nodiscard]]
        static bool check_instance_extension_support(std::vector<const char*>& instance_extensions, uint32_t& instance_extensions_count);

        [[maybe_unused]]
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback
        (
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
            void *user_data
        );

        void construct_debug_utils_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT* debug_utils_messenger_create_info);

    private:
        VkInstance m_vulkan_instance;

        std::string m_application_name;
        std::string m_engine_name;

        VkDebugUtilsMessengerEXT m_debug_messenger;
    };
}

#endif