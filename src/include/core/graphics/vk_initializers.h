#ifndef VK_INITIALIZERS_H
#define VK_INITIALIZERS_H

#include "../../../../third_party/vk_bootstrap/vk_bootstrap.h"

#include <vulkan/vulkan.h>

//Namespace for having utility functions to create vulkan objects.
//TODO : Naming convention is pretty bad, but not to be concerned with it now.
namespace halogen::vkinit
{
    namespace command_objects
    {
        VkCommandPoolCreateInfo create_command_pool_create_info(uint32_t queue_family_index = -1, VkCommandPoolCreateFlags flags = 0);
        VkCommandBufferAllocateInfo create_command_buffer_allocate_info(VkCommandPool command_pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    }
}

#endif