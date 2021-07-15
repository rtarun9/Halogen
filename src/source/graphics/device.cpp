#include "../../include/core/graphics/device.h"

namespace halogen
{
    Device::Device(VkInstance vulkan_instance, VkSurfaceKHR window_surface) : m_physical_device(VK_NULL_HANDLE), m_logical_device(VK_NULL_HANDLE), m_graphics_family_queue(VK_NULL_HANDLE), m_presentation_family_queue(VK_NULL_HANDLE)
    {
        ASSERT(vulkan_instance == VK_NULL_HANDLE, "Cannot create physical device since vulkan instance passed in is still NULL.");
        select_physical_device(vulkan_instance, window_surface);
        create_logical_device(vulkan_instance);
    }

    VkPhysicalDevice Device::get_physical_device()
    {
        return m_physical_device;
    }

    VkDevice Device::get_device()
    {
        return m_logical_device;
    }

    QueueFamilyIndices Device::get_queue_family_indices()
    {
        return m_queue_family_index;
    }

    void Device::close()
    {
        vkDestroyDevice(m_logical_device, nullptr);
    }

    Device::~Device()
    {
        close();
    }

    void Device::select_physical_device(VkInstance vulkan_instance, VkSurfaceKHR window_surface)
    {
        uint32_t physical_device_count = 0;
        std::vector<VkPhysicalDevice> physical_devices;

        vkEnumeratePhysicalDevices(vulkan_instance, &physical_device_count, nullptr);
        ASSERT(physical_device_count == 0, "Failed to find any physical devices that support vulkan. ");

        physical_devices.resize(physical_device_count);
        VK_CHECK(vkEnumeratePhysicalDevices(vulkan_instance, &physical_device_count, physical_devices.data()));

        for (VkPhysicalDevice& physical_device : physical_devices)
        {
            if (check_device_suitability(physical_device, window_surface))
            {
                m_physical_device = physical_device;
            }
        }

        ASSERT(m_physical_device == VK_NULL_HANDLE, "Failed to find suitable vulkan devices.");
    }

    void Device::create_logical_device(VkInstance vulkan_instance)
    {
        const float queue_priority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos = {};

        std::unordered_set unique_queues =
        {
            m_queue_family_index.m_graphics_queue_family.value(),
            m_queue_family_index.m_presentation_queue_family.value()
        };

        for (uint32_t queue_family : unique_queues)
        {
            VkDeviceQueueCreateInfo device_queue_create_info = {};
            device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            device_queue_create_info.pNext = nullptr;
            device_queue_create_info.queueFamilyIndex = queue_family;
            device_queue_create_info.queueCount = 1;
            device_queue_create_info.pQueuePriorities = &queue_priority;
            device_queue_create_infos.push_back(device_queue_create_info);
        }

        VkPhysicalDeviceFeatures physical_device_features = {};

        VkDeviceCreateInfo device_create_info = {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pNext = nullptr;
        device_create_info.flags = 0;
        device_create_info.queueCreateInfoCount = static_cast<uint32_t>(device_queue_create_infos.size());
        device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
        device_create_info.enabledLayerCount = configuration::VALIDATION_LAYER_COUNT;
        device_create_info.ppEnabledLayerNames = configuration::VALIDATION_LAYERS;
        device_create_info.enabledExtensionCount = configuration::DEVICE_EXTENSION_COUNT;
        device_create_info.ppEnabledExtensionNames = configuration::DEVICE_EXTENSIONS;
        device_create_info.pEnabledFeatures = &physical_device_features;

        VK_CHECK(vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_logical_device));

        vkGetDeviceQueue(m_logical_device, m_queue_family_index.m_graphics_queue_family.value(), 0, &m_graphics_family_queue);
        vkGetDeviceQueue(m_logical_device, m_queue_family_index.m_presentation_queue_family.value(), 0, &m_presentation_family_queue);
        debug::log("Device (Logical) Created. ");
    }

    bool Device::check_device_suitability(VkPhysicalDevice& physical_device, VkSurfaceKHR window_surface)
    {
        QueueFamilyIndices indices = construct_queue_family_indices(physical_device, window_surface);

        bool device_extension_support = check_device_extension_support(physical_device);

        bool swapchain_support = settings::SwapchainSettings::instance().get_swapchain_support(physical_device, window_surface);

        return indices.is_queue_family_indices_complete() && device_extension_support && swapchain_support;
    }

    bool Device::check_device_extension_support(VkPhysicalDevice &physical_device)
    {
        std::vector<VkExtensionProperties> device_extension_properties;
        uint32_t device_extension_count = 0;

        vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &device_extension_count, nullptr);
        device_extension_properties.resize(device_extension_count);

        VK_CHECK(vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &device_extension_count, device_extension_properties.data()));

        for (const char *required_device_extension : configuration::DEVICE_EXTENSIONS)
        {
            bool extension_found = false;
            for (const VkExtensionProperties& available_extension : device_extension_properties)
            {
                if (strcmp(available_extension.extensionName, required_device_extension) == 0)
                {
                    extension_found = true;
                }
            }

            if (!extension_found)
            {
                return false;
            }
        }

        return true;
    }

    QueueFamilyIndices Device::construct_queue_family_indices(VkPhysicalDevice physical_device, VkSurfaceKHR window_surface)
    {
        QueueFamilyIndices indices;

        std::vector<VkQueueFamilyProperties> queue_family_properties;
        uint32_t queue_family_property_count = 0;

        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, nullptr);
        queue_family_properties.resize(queue_family_property_count);

        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, queue_family_properties.data());

        int current_index = 0;
        for (const VkQueueFamilyProperties queue_family_property : queue_family_properties)
        {
            if (queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.m_graphics_queue_family = current_index;
            }

            VkBool32 presentation_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, current_index, window_surface, &presentation_support);
            if (presentation_support)
            {
                indices.m_presentation_queue_family = current_index;
            }

            current_index++;

            if (indices.is_queue_family_indices_complete())
            {
                m_queue_family_index = indices;
                break;
            }
        }

        return indices;
    }
}