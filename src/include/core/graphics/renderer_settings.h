#ifndef RENDERER_SETTINGS_H
#define RENDERER_SETTINGS_H

#include "vk_debug.h"
#include "../../log.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <algorithm>
#include <memory>

namespace halogen::settings
{
    /* Contains *most* details of the swapchain settings that required during swapchain creation. Acts as a link between SwapchainDetails and the actual Swapchain.
     * Currently implemented as a singleton, need to convert it into a meyers singleton later */
    struct SwapchainSettings
    {
    public:
        SwapchainSettings(SwapchainSettings const &) = delete;
        SwapchainSettings& operator=(SwapchainSettings const &) = delete;
        ~SwapchainSettings();

        static SwapchainSettings& instance();

    public:
        /* Base function to fill up the members. */
        bool get_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR window_surface);

        void set_surface_capabilies(VkSurfaceCapabilitiesKHR surface_capabilities);
        void select_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR>& surface_formats);
        void select_swapchain_present_mode(const std::vector<VkPresentModeKHR>& presentation_modes);
        void select_swapchain_extent(const VkSurfaceCapabilitiesKHR& surface_capabilies);

    public:
         VkSurfaceCapabilitiesKHR surface_setting_capabilities;
         VkSurfaceFormatKHR surface_setting_format;
         VkPresentModeKHR surface_setting_present_mode;
         VkExtent2D surface_extent;

    private:
        SwapchainSettings() {}
    };
}
#endif