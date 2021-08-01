#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace halogen
{
    struct AllocatedBuffer
    {
        //Vulkan GPU side buffer.
        VkBuffer m_buffer;

        //Hold state VMA uses (size, memory buffer was allocated from).
        VmaAllocation m_allocation;
    };
}
