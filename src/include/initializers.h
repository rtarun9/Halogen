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

	// sync object helper functions
	
	// default flag : eSignaled (since for the first frame of renderloop, we wait until the fence is signalled).
	[[nodiscard]]
	vk::FenceCreateInfo create_fence(vk::FenceCreateFlags flags = vk::FenceCreateFlagBits::eSignaled);

	// default flag : 0 (unused, preserved for future use)
	[[nodiscard]]
	vk::SemaphoreCreateInfo create_semaphore();

	// pipeline related helper functinos

	[[nodiscard]]
	vk::PipelineShaderStageCreateInfo create_shader_stage(vk::ShaderStageFlagBits shader_stage, vk::ShaderModule shader_module);
	
	[[nodiscard]]
	vk::PipelineVertexInputStateCreateInfo create_vertex_input_state();
	
	[[nodiscard]]
	vk::PipelineInputAssemblyStateCreateInfo create_input_assembler(vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList);
	
	[[nodiscard]]
	vk::PipelineRasterizationStateCreateInfo create_rasterizer_state();
	
	[[nodiscard]]
	vk::PipelineMultisampleStateCreateInfo create_multisampling_info();
	
	[[nodiscard]]
	vk::PipelineColorBlendAttachmentState create_color_blend_state();
	
	[[nodiscard]]
	VkPipelineDepthStencilStateCreateInfo create_depth_stencil_state();

	// pipeline layout : information of shader inputs
	[[nodiscard]]
	vk::PipelineLayoutCreateInfo create_pipeline_layout();
	
	// image related helper functions
	[[nodiscard]]
	VkImageCreateInfo create_image_info(VkFormat format, VkExtent3D extent, VkImageUsageFlags usage);
	
	[[nodiscard]]
	VkImageViewCreateInfo create_image_view_info(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags);
}