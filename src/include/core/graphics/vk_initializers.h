#pragma once

#include "vk_types.h"

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

    namespace image_objects
	{
    	VkImageCreateInfo create_image_create_info(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent);
    	VkImageViewCreateInfo create_image_view_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags);
	}

	//Similar to command_objects, but for pipelines.
	namespace pipeline_objects
	{
		VkShaderModuleCreateInfo create_shader_module_create_info(const std::vector<char>& code);
		VkPipelineShaderStageCreateInfo create_pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shader_module);

		VkPipelineVertexInputStateCreateInfo create_vertex_input_state_create_info();

		VkPipelineInputAssemblyStateCreateInfo create_input_assembly_state_create_info(VkPrimitiveTopology topology);
		VkPipelineRasterizationStateCreateInfo create_rasterization_state_create_info(VkPolygonMode polygon_mode);
		VkPipelineMultisampleStateCreateInfo create_multisample_state_create_info();
		VkPipelineColorBlendAttachmentState create_color_blend_attachment_state();
		VkPipelineDepthStencilStateCreateInfo create_depth_stencil_state_create_info(bool enable_depth_test, bool depth_write, VkCompareOp compare_op);

		VkPipelineLayoutCreateInfo create_pipeline_layout_create_info();
	}

	namespace buffers
	{
		AllocatedBuffer create_buffer(size_t buffer_size, VkBufferUsageFlagBits usage_flags, VmaMemoryUsage memory_usage, VmaAllocator allocator);
	}
}
