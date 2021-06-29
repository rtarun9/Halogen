#ifndef RENDERER_H
#define RENDERER_H

#include "../../configuration.h"
#include "../../core/platform.h"
#include "../../core/graphics/vk_debug.h"

#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <optional>
#include <set>
#include <vector>

namespace halogen
{
    /* Struct for the queue families indices.
     * Present plan is to only have two families, for presentation and graphics (both will
     * most probably overlap). */
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> m_graphics_queue_family;
        std::optional<uint32_t> m_presentation_queue_family;
        bool is_queue_family_complete()
        {
            return m_graphics_queue_family.has_value() && m_presentation_queue_family.has_value();
        }
    };

    /* Main renderer of the engine. */
    class Renderer
    {
    public:
        /* Core Renderer functions. */
        Renderer(std::shared_ptr<Window>& m_window_reference);

        void initialize_renderer();
        void render();

        void cleanup();
        ~Renderer();

    private:
        /* Vulkan related core functions */
        void initialize_vulkan();

        void create_vulkan_instance();
        void setup_debug_messenger();
        void create_window_surface();
        void select_physical_device();
        void create_device();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback
        (
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
            void *user_data
        );

        /* Vulkan helper functions. */
        void check_extension_support(std::vector<const char*>& instance_extension_names, uint32_t& instance_extension_count);
        void get_instance_extensions(std::vector<const char*>& instance_extension_names, uint32_t& instance_extension_count);

        VkResult  create_platform_specific_window_surface();
        bool check_validation_layer_support();

        void construct_debug_utils_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback);

        void get_supported_physical_devices(std::vector<VkPhysicalDevice>& available_physical_devices, uint32_t& available_physical_device_count);
        bool is_physical_device_suitable(VkPhysicalDevice& physical_device);
        QueueFamilyIndices get_queue_family_indices(VkPhysicalDevice& physical_device);

        /* Pointer to vulkan functions. */
        VkResult create_debug_utils_messenger_EXT
        (
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT *create_info,
            const  VkAllocationCallbacks *allocator,
            VkDebugUtilsMessengerEXT *messenger
        );
        void destroy_debug_utils_messenger_ext
        (
            VkInstance instance,
            VkDebugUtilsMessengerEXT messenger,
            const VkAllocationCallbacks *allocator
        );

    private:
        /* Core vulkan variables. */
        VkInstance m_vulkan_instance;
        VkSurfaceKHR m_window_surface;
        VkPhysicalDevice m_physical_device;
        VkDevice m_device;
        VkDebugUtilsMessengerEXT m_debug_messenger;

        /* Queue handles. */
        VkQueue m_graphics_queue;
        VkQueue m_presentation_queue;

        /* References to other classes*/
        std::shared_ptr<Window>& m_window_reference;
    };
}

#endif