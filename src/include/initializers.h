#pragma once

#include "types.h"

// use of initializers.h : Contains utility functions to create vulkan handles
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
	vk::PipelineDepthStencilStateCreateInfo create_depth_stencil_state();

	// pipeline layout : information of shader inputs
	[[nodiscard]]
	vk::PipelineLayoutCreateInfo create_pipeline_layout();
	
	// image related helper functions
	[[nodiscard]]
	vk::ImageCreateInfo create_image_info(vk::Format format, vk::Extent3D extent, vk::ImageUsageFlagBits usage);

	[[nodiscard]]
	vk::ImageViewCreateInfo create_image_view_info(vk::Format format, vk::Image image, vk::ImageAspectFlagBits aspect_flags);

	// helper functions for descriptor sets
	[[nodiscard]]
	vk::DescriptorSetLayoutBinding create_descriptor_set_layout_binding(vk::DescriptorType type, vk::ShaderStageFlags shader_stage, uint32_t binding);

	// the descriptor pool is used to allcate descriptors from it
	// vk::DescriptorPoolSize is a struct having the type and the maximum number of pointers to that type of descriptor that can be allocated from it.
	[[nodiscard]]
	vk::DescriptorPoolCreateInfo create_descriptor_pool(const std::vector<vk::DescriptorPoolSize>& pool_size, uint32_t max_descriptor_set_count);
	
	[[nodiscard]]
	vk::WriteDescriptorSet write_descriptor_buffer(vk::DescriptorType type, vk::DescriptorSet descriptor_set, vk::DescriptorBufferInfo *buffer_info, uint32_t binding);
}