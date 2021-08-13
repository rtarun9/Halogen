#include "../../include/core/graphics/vk_initializers.h"

namespace halogen::vkinit
{
    VkCommandPoolCreateInfo command_objects::create_command_pool_create_info(uint32_t queue_family_index, VkCommandPoolCreateFlags flags)
    {
        VkCommandPoolCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        create_info.pNext = nullptr;
        create_info.flags = flags;
        create_info.queueFamilyIndex = queue_family_index;

        return create_info;
    }

    VkCommandBufferAllocateInfo command_objects::create_command_buffer_allocate_info(VkCommandPool command_pool, uint32_t count, VkCommandBufferLevel level)
    {
        VkCommandBufferAllocateInfo allocate_info = {};
        allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.pNext = nullptr;
        allocate_info.commandPool = command_pool;
        allocate_info.commandBufferCount = count;
        allocate_info.level = level;
        return allocate_info;
    }

    VkImageCreateInfo image_objects::create_image_create_info(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent)
	{
		VkImageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.imageType = VK_IMAGE_TYPE_2D;
		create_info.format = format;

		//Size of image in pixels.
		create_info.extent = extent;
		create_info.mipLevels = 1;
		create_info.arrayLayers = 1;

		//No MSAA
		create_info.samples = VK_SAMPLE_COUNT_1_BIT;

		//Tiling describes how data for texture is arranged in GPU. Using VK_IMAGE_TILING_OPTIMAL lets driver decide how
		//GPU arranges memory of image
		//[NOTE : ] By using TILING_OPTIMAL you cant read data from CPU without changing tiling mode first.
		create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		create_info.usage = usage_flags;

		return create_info;
	}

	VkImageViewCreateInfo image_objects::create_image_view_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.image = image;
		create_info.format = format;

		//Information about where image point to : used because image view wrap an image, and its necessary to point to the original one.
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;

		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		create_info.subresourceRange.aspectMask = aspect_flags;

		return create_info;
	}
}