#include "../include/vk_initializers.h"

namespace halo::init
{
	VkCommandPoolCreateInfo create_command_pool(uint32_t queue_family_index, VkCommandPoolCreateFlags flags)
	{
		VkCommandPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = flags;
		create_info.queueFamilyIndex = queue_family_index;
		
		return create_info;
	}

	VkCommandBufferAllocateInfo init::create_command_buffer_allocate(VkCommandPool command_pool, VkCommandBufferLevel level)
	{
		VkCommandBufferAllocateInfo allocate_info = {};
		allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocate_info.pNext = nullptr;
		allocate_info.commandBufferCount = 1;
		allocate_info.commandPool = command_pool;
		allocate_info.level = level;

		return allocate_info;
	}
}