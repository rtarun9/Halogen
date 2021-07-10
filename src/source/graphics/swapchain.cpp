#include "../../include/core/graphics/swapchain.h"

namespace halogen
{
    SwapchainDetails::SwapchainDetails(VkSurfaceKHR window_surface, VkPhysicalDevice physical_device)
    {
        get_swapchain_support(window_surface, physical_device);
    }

    void SwapchainDetails::get_swapchain_support(VkSurfaceKHR window_surface, VkPhysicalDevice physical_device)
    {
        /* Get base device capabilities like min/max image count, extent, transform etc. */
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, window_surface, &m_surface_capabilities));

        /* Get surface format details (Color space, SRGB etc). */
        uint32_t surface_format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, window_surface, &surface_format_count, nullptr);
        ASSERT(surface_format_count == 0, "Failed to find any surface formats. ");

        m_surface_formats.resize(surface_format_count);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, window_surface, &surface_format_count, m_surface_formats.data()));

        /* Get presentation modes (most important swapchain setup variable) : fifo / fifo relaxed / mailbox etc. */
        uint32_t surface_present_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, window_surface, &surface_present_mode_count, nullptr);
        ASSERT(surface_present_mode_count == 0, "Failed to find any surface presentation modes. ");

        m_surface_present_modes.resize(surface_present_mode_count);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, window_surface, &surface_present_mode_count, m_surface_present_modes.data()));
    }

    bool SwapchainDetails::check_swapchain_device_support(VkPhysicalDevice physical_device)
    {
        bool swapchain_device_support = false;

        swapchain_device_support = !(m_surface_present_modes.empty() && m_surface_formats.empty());
        return swapchain_device_support;
    }

    SwapchainDetails::~SwapchainDetails()
    {
    }

    SwapchainSettings::SwapchainSettings()
    {
    }

    void SwapchainSettings::set_swapchain_setting(const SwapchainDetails &swapchain_details)
    {
        set_surface_capabilies(swapchain_details.m_surface_capabilities);
        select_swapchain_surface_format(swapchain_details.m_surface_formats);
        select_swapchain_present_mode(swapchain_details.m_surface_present_modes);
        select_swapchain_extent(swapchain_details.m_surface_capabilities);
    }

    SwapchainSettings::~SwapchainSettings()
    {
    }

    /* Only reason this exists is so that the swapchain base class can get details to some fundamental surface capabilities. */
    void SwapchainSettings::set_surface_capabilies(VkSurfaceCapabilitiesKHR surface_capabilities)
    {
        m_surface_setting_capabilities = surface_capabilities;
    }

    void SwapchainSettings::select_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR> &surface_formats)
    {
        for (const VkSurfaceFormatKHR& available_surface_format : surface_formats)
        {
            if (available_surface_format.format == VK_FORMAT_R8G8B8A8_SRGB && available_surface_format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
            {
                m_surface_setting_format = available_surface_format;
                return;
            }
        }

        m_surface_setting_format = surface_formats[0];
    }

    void SwapchainSettings::select_swapchain_present_mode(const std::vector<VkPresentModeKHR> &presentation_modes)
    {
        for (const VkPresentModeKHR& available_presentation_mode : presentation_modes)
        {
            if (available_presentation_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                m_surface_setting_present_mode = available_presentation_mode;
                return;
            }
        }

        m_surface_setting_present_mode = VK_PRESENT_MODE_FIFO_KHR;
    }

    void SwapchainSettings::select_swapchain_extent(const VkSurfaceCapabilitiesKHR &surface_capabilies)
    {
        if (surface_capabilies.currentExtent.width != UINT32_MAX || surface_capabilies.currentExtent.height != UINT32_MAX)
        {
            m_surface_extent = surface_capabilies.currentExtent;
            return;
        }


        m_surface_extent.width = std::clamp(m_surface_extent.width, surface_capabilies.minImageExtent.width, surface_capabilies.maxImageExtent.width);
        m_surface_extent.height = std::clamp(m_surface_extent.height, surface_capabilies.minImageExtent.width, surface_capabilies.maxImageExtent.height);
    }

    Swapchain::Swapchain(VkInstance instance, std::unique_ptr<Device>& device,  VkSurfaceKHR window_surface) : m_vulkan_instance(instance), m_device(device->get_device()), m_window_surface(window_surface)
    {
        m_swapchain_details = SwapchainDetails(window_surface, device->get_physical_device());
        m_swapchain_settings = SwapchainSettings();

        m_swapchain_settings.set_swapchain_setting(m_swapchain_details);

        create_swapchain(device);
    }

    void Swapchain::create_swapchain(std::unique_ptr<Device>& device)
    {
        uint32_t image_count = m_swapchain_settings.m_surface_setting_capabilities.minImageCount + 1;
        uint32_t max_image_count = m_swapchain_settings.m_surface_setting_capabilities.maxImageCount;

        if (max_image_count > 0 && image_count > max_image_count)
        {
            image_count = max_image_count;
        }

        VkSwapchainCreateInfoKHR swapchain_create_info = {};
        swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_create_info.surface = m_window_surface;
        swapchain_create_info.imageFormat = m_swapchain_settings.m_surface_setting_format.format;
        swapchain_create_info.imageColorSpace = m_swapchain_settings.m_surface_setting_format.colorSpace;
        swapchain_create_info.imageExtent = m_swapchain_settings.m_surface_extent;
        swapchain_create_info.imageArrayLayers = 1;
        swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = device->get_queue_family_indices();
        uint32_t queue_family_indices[] = {indices.m_graphics_queue_family.value(), indices.m_presentation_queue_family.value()};

    }
}