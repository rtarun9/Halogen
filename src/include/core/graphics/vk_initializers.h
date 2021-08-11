#pragma once

#include <vk_bootstrap.h>
#include <vk_mem_alloc.h>

#include <vulkan/vulkan.h>

//utility functions to help dynamically create some vulkan objects.
namespace halogen::vkinit
{
    namespace command_objects
    {
        VkCommandPoolCreateInfo create_command_pool_create_info(uint32_t queue_family_index = -1, VkCommandPoolCreateFlags flags = 0);
        VkCommandBufferAllocateInfo create_command_buffer_allocate_info(VkCommandPool command_pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    }
}
