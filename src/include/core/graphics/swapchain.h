#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "vk_debug.h"
#include "../../log.h"
#include "../../configuration.h"
#include "vk_external.h"
#include "../platform.h"
#include "device.h"

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <algorithm>

namespace halogen
{
    class Swapchain
    {
    public:
        Swapchain(VkInstance instance, VkDevice device, QueueFamilyIndices queue_family_index, VkSurfaceKHR window_surface);

        void create_swapchain(VkDevice& device, QueueFamilyIndices& queue_family_indices);
        void create_swapchain_image_views(VkDevice& device);

        ~Swapchain();

    private:
        VkSwapchainKHR m_swapchain;

        /* Reference to the surface and instance. */
        VkInstance m_vulkan_instance;
        VkSurfaceKHR m_window_surface;
        VkDevice m_device;

        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;
        VkFormat swapchain_image_format;
    };
}

#endif