#ifndef RENDERER_H
#define RENDERER_H

#include "../../configuration.h"
#include "../../core/platform.h"
#include "../../core/graphics/vk_debug.h"
#include "../../common.h"

#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <optional>
#include <set>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <utility>
#include <string>

namespace halogen
{
    /* Struct for the queue families indices.
     * Present plan is to only have two families, for presentation and graphics (both will
     * most probably overlap).
     * [Note] : remove presentation queue family later since its more of a graphics queue family feature than a
     * exclusive queue family. */
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> m_graphics_queue_family;
        std::optional<uint32_t> m_presentation_queue_family;
        bool is_queue_family_complete()
        {
            return m_graphics_queue_family.has_value() && m_presentation_queue_family.has_value();
        }
    };

    /* Struct for the swap chain support details.
     * Will check if the swap chain is compatible with the window surface.
     * Also will check which features are supported and set up recommended / fallback features for the swapchain. */

    struct SwapchainDetails
    {
        VkSurfaceCapabilitiesKHR m_surface_capabilites;
        std::vector<VkSurfaceFormatKHR> m_surface_formats;
        std::vector<VkPresentModeKHR> m_presentation_modes;

        bool is_swapchain_supported()
        {
            return !m_surface_formats.empty() && !m_presentation_modes.empty();
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
        void create_swapchain();
        void create_swapchain_image_views();
        void create_graphics_pipeline();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback
        (
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
            void *user_data
        );

        /* Vulkan helper functions. */
        void check_instance_extension_support(std::vector<const char*>& instance_extension_names, uint32_t& instance_extension_count);
        void get_instance_extensions(std::vector<const char*>& instance_extension_names, uint32_t& instance_extension_count);

        bool check_device_extension_support(VkPhysicalDevice& device);

        VkResult  create_platform_specific_window_surface();
        bool check_validation_layer_support();

        void construct_debug_utils_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback);

        void get_supported_physical_devices(std::vector<VkPhysicalDevice>& available_physical_devices, uint32_t& available_physical_device_count);
        bool is_physical_device_suitable(VkPhysicalDevice& physical_device);
        QueueFamilyIndices get_queue_family_indices(VkPhysicalDevice& physical_device);

        SwapchainDetails get_swapchain_support_details(VkPhysicalDevice& device);

        /* Helper functions for choosing optimal / fallback swapchain settings. */
        VkSurfaceFormatKHR select_preferred_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR>& availabe_surface_formats);
        VkPresentModeKHR select_preferred_swapchain_presentation_mode(const std::vector<VkPresentModeKHR>& available_presentation_modes);
        VkExtent2D select_swapchain_extent(const VkSurfaceCapabilitiesKHR& surface_capabilites);


        /* Helper functions for the graphics pipeline. */
        std::pair<VkShaderModule, VkShaderModule> create_shader_modules(const char *vertex_shader_file_path, const char *fragment_shader_file_path);

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
        VkSwapchainKHR m_swapchain;
        VkDebugUtilsMessengerEXT m_debug_messenger;

        /* Swapchain related variables. */
        VkPresentModeKHR m_swapchain_presentation_mode;
        VkExtent2D m_swapchain_extent;
        VkSurfaceFormatKHR m_swapchain_surface_format;

        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;

        /* Queue handles. */
        VkQueue m_graphics_queue;
        VkQueue m_presentation_queue;

        /* References to other classes*/
        std::shared_ptr<Window>& m_window_reference;
    };
}

#endif