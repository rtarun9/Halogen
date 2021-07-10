#ifndef DEVICE_H
#define DEVICE_H

#include "vk_debug.h"
#include "../../log.h"
#include "../../configuration.h"
#include "vk_external.h"
#include "../platform.h"
#include "instance.h"
#include "swapchain.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <optional>

class halogen::SwapchainDetails;

namespace halogen
{
    class QueueFamilyIndices
    {
    public:
        std::optional<uint32_t> m_graphics_queue_family;
        std::optional<uint32_t> m_presentation_queue_family;

        [[nodiscard]]
        bool is_queue_family_indices_complete()
        {
         return m_graphics_queue_family.has_value(); //&& m_presentation_queue_family.has_value();
        }
    };

    /* Class for both logical and physical devices. Also has some handles for the respective queues. */
    class Device
    {
    public:
        Device(VkInstance vulkan_instance, VkSurfaceKHR window_surface);

        [[nodiscard]]
        VkPhysicalDevice get_physical_device();

        [[nodiscard]]
        VkDevice get_device();

        [[nodiscard]]
        QueueFamilyIndices get_queue_family_indices();

        void close();
        ~Device();

    private:
        void select_physical_device(VkInstance vulkan_instance, VkSurfaceKHR window_surface);
        void create_logical_device(VkInstance vulkan_instance);

        [[nodiscard]]
        bool check_device_suitability(VkPhysicalDevice& physical_device, VkSurfaceKHR window_surface);

        [[nodiscard]]
        bool check_device_extension_support(VkPhysicalDevice& physical_device);

        QueueFamilyIndices construct_queue_family_indices(VkPhysicalDevice physical_device);

    private:
        VkPhysicalDevice m_physical_device;
        VkDevice m_logical_device;

        /* This is so that we do not need to keep enquiring about the queue family indices whenever it needs to be acquired. */
        QueueFamilyIndices m_queue_family_index;

        VkQueue m_graphics_family_queue;
        VkQueue m_presentation_family_queue;
    };
}
#endif