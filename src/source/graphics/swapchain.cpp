#include "../../include/core/graphics/swapchain.h"

namespace halogen
{
    Swapchain::Swapchain(VkInstance instance, VkDevice device, QueueFamilyIndices queue_family_index,  VkSurfaceKHR window_surface) : m_vulkan_instance(instance), m_device(device), m_window_surface(window_surface)
    {
        create_swapchain(device, queue_family_index);
        create_swapchain_image_views(device);
    }

    void Swapchain::create_swapchain(VkDevice& device, QueueFamilyIndices& queue_family_index)
    {
        uint32_t image_count = settings::SwapchainSettings::instance().surface_setting_capabilities.minImageCount + 1;
        uint32_t max_image_count = settings::SwapchainSettings::instance().surface_setting_capabilities.maxImageCount;

        if (max_image_count > 0 && image_count > max_image_count)
        {
            image_count = max_image_count;
        }

        VkSwapchainCreateInfoKHR swapchain_create_info = {};
        swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_create_info.surface = m_window_surface;
        swapchain_create_info.minImageCount = image_count;
        swapchain_create_info.imageFormat = settings::SwapchainSettings::instance().surface_setting_format.format;
        swapchain_create_info.imageColorSpace = settings::SwapchainSettings::instance().surface_setting_format.colorSpace;
        swapchain_create_info.imageExtent = settings::SwapchainSettings::instance().surface_extent;
        swapchain_create_info.imageArrayLayers = 1;
        swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        /* Move this onto the swapchain settings static class later on. */

        std::vector<uint32_t> queue_family_indices;
        queue_family_index.get_queue_families(queue_family_indices);

        if (queue_family_index.are_graphics_and_presentation_queue_equal())
        {
            swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchain_create_info.queueFamilyIndexCount = 0;
            swapchain_create_info.pQueueFamilyIndices = nullptr;
        }
        else
        {
            swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchain_create_info.queueFamilyIndexCount = queue_family_indices.size();
            swapchain_create_info.pQueueFamilyIndices = queue_family_indices.data();
        }

        swapchain_create_info.preTransform = settings::SwapchainSettings::instance().surface_setting_capabilities.currentTransform;
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_create_info.presentMode = settings::SwapchainSettings::instance().surface_setting_present_mode;
        swapchain_create_info.clipped = VK_TRUE;
        swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

        VK_CHECK(vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &m_swapchain));

        debug::log("Created Swapchain !");

        vkGetSwapchainImagesKHR(device, m_swapchain, &image_count, nullptr);
        m_swapchain_images.resize(image_count);

        VK_CHECK(vkGetSwapchainImagesKHR(device, m_swapchain, &image_count, m_swapchain_images.data()));
    }

    void Swapchain::create_swapchain_image_views(VkDevice &device)
    {
        m_swapchain_image_views.resize(m_swapchain_images.size());

        for (size_t i = 0; i < m_swapchain_images.size(); i++)
        {
            VkImageViewCreateInfo image_view_create_info = {};
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.image = m_swapchain_images[i];
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = settings::SwapchainSettings::instance().surface_setting_format.format;

            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.baseMipLevel = 0;
            image_view_create_info.subresourceRange.levelCount = 1;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount = 1;

            VK_CHECK(vkCreateImageView(device, &image_view_create_info, nullptr, &m_swapchain_image_views[i]));
        }

        debug::log("Swapchain image views created. ");
    }

    Swapchain::~Swapchain()
    {
        for (size_t i = 0; i < m_swapchain_image_views.size(); i++)
        {
            vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);
        }

        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    }
}