#ifndef VK_DEBUG_UTILS
#define VK_DEBUG_UTILS

#include "../../log.h"

#include <vulkan/vulkan.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/* Vulkan specific debugging utilities. */

/* TODO : missing some error checks regarding swapchain info (since program throws error but not caught be VK_CHCK. */
/* Note : Convert to macro once function is complete.
 * Function currently not exiting program : try to do this without throwing exception (in future). */
inline void VK_CHECK(VkResult result, std::string additional_message = "")
{
    std::stringstream error_string_stream;
    if (result == VK_SUCCESS)
    {
        return;
    }
    else if (result == VK_ERROR_EXTENSION_NOT_PRESENT)
    {
        error_string_stream  << " Extensions not found!" << '\n';
    }
    else if (result == VK_ERROR_VALIDATION_FAILED_EXT)
    {
        error_string_stream << " Validation failed!" << '\n';
    }
    else if (result == VK_ERROR_LAYER_NOT_PRESENT)
    {
        error_string_stream << "Requested validation layer not present!" << '\n';
    }
    else if (result == VK_ERROR_FEATURE_NOT_PRESENT)
    {
        error_string_stream <<  " Requested feature not present" << '\n';
    }
    else if (result == VK_ERROR_UNKNOWN)
    {
        error_string_stream  << " error type unknown. Line in code : " << __LINE__ << '\n';
    }
    else if (result == VK_ERROR_INITIALIZATION_FAILED)
    {
        error_string_stream <<  " initialization of system failed. Line in code : " << __LINE__ << '\n';
    }
    else if (result == VK_ERROR_OUT_OF_DEVICE_MEMORY)
    {
        error_string_stream <<  " device is out of memory!" << '\n';
    }
    else if (result == VK_ERROR_OUT_OF_HOST_MEMORY)
    {
        error_string_stream  << " out of host memory!" << '\n';
    }
    else if (result == VK_INCOMPLETE)
    {
        error_string_stream  << " requested validation layers not found. " << '\n';
    }
    else
    {
        error_string_stream << "[ERROR TYPE NOT FOUND]. Line : " << __LINE__ << '\n';
    }

    error_string_stream << " " << additional_message;

    halogen::debug::error(error_string_stream.str().c_str());
}

namespace vk_debug
{
    static void display_available_extension(std::vector<VkExtensionProperties>& extensions)
    {
        std::cout << '\n' << "Available extensions : " << '\n';
        for (const VkExtensionProperties& extension : extensions)
        {
            std::cout << '\t' << extension.extensionName << '\n';
        }
    }

    static void display_available_layers(std::vector<VkLayerProperties>& layers)
    {
        std::cout << '\n' << "Available layers : " << '\n';
        for (const VkLayerProperties& layer : layers)
        {
            std::cout << '\t' << layer.layerName << '\n';
        }
    }

    static void display_physical_device_details(VkPhysicalDevice& physical_device)
    {
        VkPhysicalDeviceProperties physical_device_properties = {};
        vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

        std::cout << '\n' << " Device Name : " << physical_device_properties.deviceName << '\n'
                  << " Driver Version : " << physical_device_properties.driverVersion << '\n';
    }
}

#endif