#include "../../include/core/graphics/renderer_settings.h"
#include "../../include/core/graphics/swapchain.h"

namespace halogen::settings
{

    SwapchainSettings& SwapchainSettings::instance()
    {
        static const std::unique_ptr<SwapchainSettings> instance{new SwapchainSettings()};
        return *instance;
    }

    SwapchainSettings::~SwapchainSettings()
    {
    }

    bool SwapchainSettings::get_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR window_surface)
    {
        /* Get base device capabilities like min/max image count, extent, transform etc. */
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, window_surface, &surface_setting_capabilities));

        /* Get surface format details (Color space, SRGB etc). */
        uint32_t surface_format_count = 0;
        std::vector<VkSurfaceFormatKHR> surface_formats;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, window_surface, &surface_format_count, nullptr);
        ASSERT(surface_format_count == 0, "Failed to find any surface formats. ");

        surface_formats.resize(surface_format_count);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, window_surface, &surface_format_count, surface_formats.data()));

        /* Get presentation modes (most important swapchain setup variable) : fifo / fifo relaxed / mailbox etc. */
        uint32_t surface_present_mode_count = 0;
        std::vector<VkPresentModeKHR> surface_present_modes;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, window_surface, &surface_present_mode_count, nullptr);
        ASSERT(surface_present_mode_count == 0, "Failed to find any surface presentation modes. ");

        surface_present_modes.resize(surface_present_mode_count);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, window_surface, &surface_present_mode_count, surface_present_modes.data()));

        if (surface_present_mode_count == 0 || surface_format_count == 0)
        {
            debug::error("Could not create swapchain since physical device canno support it.");
            return false;
        }
        else
        {
            //if all the conditions are met, set up the choose which settings are optimal.
            set_surface_capabilies(surface_setting_capabilities);
            select_swapchain_surface_format(surface_formats);
            select_swapchain_present_mode(surface_present_modes);

            select_swapchain_extent(surface_setting_capabilities);
        }

        return true;
    }



    /* Only reason this exists is so that the swapchain base class can get details to some fundamental surface capabilities. */
    void SwapchainSettings::set_surface_capabilies(VkSurfaceCapabilitiesKHR surface_capabilities)
    {
        surface_setting_capabilities = surface_capabilities;
    }

    void SwapchainSettings::select_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR> &surface_formats)
    {
        for (const VkSurfaceFormatKHR& available_surface_format : surface_formats)
        {
            if (available_surface_format.format == VK_FORMAT_R8G8B8A8_SRGB && available_surface_format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
            {
                surface_setting_format = available_surface_format;
                return;
            }
        }

        surface_setting_format = surface_formats[0];
    }

    void SwapchainSettings::select_swapchain_present_mode(const std::vector<VkPresentModeKHR> &presentation_modes)
    {
        for (const VkPresentModeKHR& available_presentation_mode : presentation_modes)
        {
            if (available_presentation_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                surface_setting_present_mode = available_presentation_mode;
                return;
            }
        }

        surface_setting_present_mode = VK_PRESENT_MODE_FIFO_KHR;
    }

    void SwapchainSettings::select_swapchain_extent(const VkSurfaceCapabilitiesKHR &surface_capabilies)
    {
        if (surface_capabilies.currentExtent.width != UINT32_MAX || surface_capabilies.currentExtent.height != UINT32_MAX)
        {
            surface_extent = surface_capabilies.currentExtent;
            return;
        }

        surface_extent.width = std::clamp(surface_extent.width, surface_capabilies.minImageExtent.width, surface_capabilies.maxImageExtent.width);
        surface_extent.height = std::clamp(surface_extent.height, surface_capabilies.minImageExtent.width, surface_capabilies.maxImageExtent.height);
    }

}