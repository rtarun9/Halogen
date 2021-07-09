#ifndef VK_EXTERNAL_H
#define VK_EXTERNAL_H

namespace halogen::vk_external
{
    static VkResult create_debug_utils_messenger_EXT
    (
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT *create_info,
        const  VkAllocationCallbacks *allocator,
        VkDebugUtilsMessengerEXT *messenger
    )
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func == nullptr)
        {
            halogen::debug::error("Could not load vkCreateDebugUtilsMessengerEXT");
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        else
        {
            return func(instance, create_info, allocator, messenger);
        }
    }

    static void destroy_debug_utils_messenger_ext
    (
        VkInstance instance,
        VkDebugUtilsMessengerEXT messenger,
        const VkAllocationCallbacks *allocator
    )
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func == nullptr)
        {
            halogen::debug::error("Could not load vkDestroyDebugUtilsMessengerEXT");
        }

        return func(instance, messenger, allocator);
    }
}

#endif