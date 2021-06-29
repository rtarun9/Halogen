#ifndef VK_DEBUG_UTILS
#define VK_DEBUG_UTILS

/* Included iostream since this code shouldn't be connected to halogen's logger system*/
#include <vulkan/vulkan.h>
#include <iostream>

/* Vulkan specific debugging utilities. */

/* Note : Convert to macro once function is complete.
 * Function currently not exiting program : try to do this without throwing exception (in future). */
inline void VK_CHECK(VkResult result)
{
    if (result == VK_SUCCESS)
    {
        return;
    }
    else if (result == VK_ERROR_EXTENSION_NOT_PRESENT)
    {
        std::cerr << "[ERROR : ]" << " Extensions not found!" << '\n';
    }
    else if (result == VK_ERROR_VALIDATION_FAILED_EXT)
    {
        std::cerr << "[ERROR : ]" << " Validation failed!" << '\n';
    }
    else if (result == VK_ERROR_LAYER_NOT_PRESENT)
    {
        std::cerr << "[ERROR : ]" << "Requested validation layer not present!" << '\n';
    }
    else if (result == VK_ERROR_FEATURE_NOT_PRESENT)
    {
        std::cerr << "[ERROR : ]" << " Requested feature not present" << '\n';
    }
    else if (result == VK_ERROR_UNKNOWN)
    {
        std::cerr << "[ERROR : ]" << " error type unknown. Line in code : " << __LINE__ << '\n';
    }
    else if (result == VK_ERROR_INITIALIZATION_FAILED)
    {
        std::cerr << "[ERROR : ]" << " initialization of system failed. Line in code : " << __LINE__ << '\n';
    }
    else if (result == VK_ERROR_OUT_OF_DEVICE_MEMORY)
    {
        std::cerr << "[ERROR : ]" << " device is out of memory!" << '\n';
    }
    else if (result == VK_ERROR_OUT_OF_HOST_MEMORY)
    {
        std::cerr << "[ERROR : ]" << " out of host memory!" << '\n';
    }
    else
    {
        std::cerr << "[ERROR TYPE NOT FOUND]. Line : " << __LINE__ << '\n';
    }
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