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
    /* Classes for both the swapchain wrapper and the Swapchain details. */
    class SwapchainDetails
    {
    public:
        SwapchainDetails(VkSurfaceKHR window_surface = VK_NULL_HANDLE, VkPhysicalDevice physical_device = VK_NULL_HANDLE);

        /* Base function to fill up the members. */
        void get_swapchain_support(VkSurfaceKHR window_surface, VkPhysicalDevice physical_device);

        [[nodiscard]]
        bool check_swapchain_device_support(VkPhysicalDevice physical_device);

        ~SwapchainDetails();

    public:
        VkSurfaceCapabilitiesKHR m_surface_capabilities;
        std::vector<VkSurfaceFormatKHR> m_surface_formats;
        std::vector<VkPresentModeKHR> m_surface_present_modes;
    };

    /* Contains *most* details of the swapchain settings that required during swapchain creation. Acts as a link between SwapchainDetails and the actual Swapchain. */

    struct SwapchainSettings
    {
    public:
        SwapchainSettings();

        void set_swapchain_setting(const SwapchainDetails& swapchain_details);

        ~SwapchainSettings();

    private:
        void set_surface_capabilies(VkSurfaceCapabilitiesKHR surface_capabilities);
        void select_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats);
        void select_swapchain_present_mode(const std::vector<VkPresentModeKHR>& presentation_modes);
        void select_swapchain_extent(const VkSurfaceCapabilitiesKHR& surface_capabilies);

    public:
        VkSurfaceCapabilitiesKHR m_surface_setting_capabilities;
        VkSurfaceFormatKHR m_surface_setting_format;
        VkPresentModeKHR m_surface_setting_present_mode;
        VkExtent2D m_surface_extent;
    };

    class Swapchain
    {
    public:
        Swapchain(VkInstance instance, std::unique_ptr<Device>& device, VkSurfaceKHR window_surface, );

        void create_swapchain(std::unique_ptr<Device>& device);

        ~Swapchain();

    private:
        VkSwapchainKHR m_swapchain;

        /* Reference to the surface and instance. */
        VkInstance m_vulkan_instance;
        VkSurfaceKHR m_window_surface;
        VkDevice m_device;

        /**/
        SwapchainDetails m_swapchain_details;
        SwapchainSettings m_swapchain_settings;

    };
}

#endif