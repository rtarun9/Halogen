#pragma once

#include "vk_types.h"

namespace halo::init
{
	// default flag : VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, for allowing individual command buffers allocated from this pool to be reset.
	VkCommandPoolCreateInfo create_command_pool(uint32_t queue_family_index, VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	
	// default level : VK_COMMAND_BUFFER_LEVEL_PRIMARY : Since they are mainly used for submission to queues.
	VkCommandBufferAllocateInfo create_command_buffer_allocate(VkCommandPool command_pool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}