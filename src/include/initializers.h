#pragma once

#include "types.h"

namespace halo::init
{
	// default flag : vk::CommandPoolCreateFlagBits::eResetCommandBuffer for allowing individual command buffers allocated from this pool to be reset.
	[[nodiscard]]
	vk::CommandPoolCreateInfo create_command_pool(uint32_t queue_family_index, vk::CommandPoolCreateFlags flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	
	// default level : vk::CommandBufferLevel::ePrimary : Since they are mainly used for submission to queues.
	[[nodiscard]]
	vk::CommandBufferAllocateInfo create_command_buffer_allocate(vk::CommandPool command_pool, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);

	// pipeline related helper functinos

	[[nodiscard]]
	VkPipelineShaderStageCreateInfo create_shader_stage(VkShaderStageFlagBits shader_stage, VkShaderModule shader_module);
	
	[[nodiscard]]
	VkPipelineVertexInputStateCreateInfo create_vertex_input_state();
	
	[[nodiscard]]
	VkPipelineInputAssemblyStateCreateInfo create_input_assembler();
	
	[[nodiscard]]
	VkPipelineRasterizationStateCreateInfo create_rasterizer_state();
	
	[[nodiscard]]
	VkPipelineMultisampleStateCreateInfo create_multisampling_info();
	
	[[nodiscard]]
	VkPipelineColorBlendAttachmentState create_color_blend_state();
	
	[[nodiscard]]
	VkPipelineDepthStencilStateCreateInfo create_depth_stencil_state();

	// pipeline layout : information of shader inputs
	[[nodiscard]]
	VkPipelineLayoutCreateInfo create_pipeline_layout();
	
	// image related helper functions
	[[nodiscard]]
	VkImageCreateInfo create_image_info(VkFormat format, VkExtent3D extent, VkImageUsageFlags usage);
	
	[[nodiscard]]
	VkImageViewCreateInfo create_image_view_info(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags);
}