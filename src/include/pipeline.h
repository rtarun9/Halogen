#pragma once

#include "types.h"

#include <vector>

namespace halo
{
	class PipelineBuilder
	{
	public:
		[[nodiscard]]
		vk::Pipeline create_pipeline(vk::Device device, vk::RenderPass renderpass);

	public:

		// contains all shader stages (vertex, fragment, compute, etc)
		std::vector<vk::PipelineShaderStageCreateInfo> m_shader_stages;

		// contains info of the vertex buffer and its formats (specify how data is laid out and how to interpret it). It's vulkan's VAO
		vk::PipelineVertexInputStateCreateInfo m_vertex_input_info;
		
		// contains info of the topology to be used while drawing
		vk::PipelineInputAssemblyStateCreateInfo m_input_assembler;
		
		vk::Viewport m_viewport;
		vk::Rect2D m_scissor;

		// configuration of fixed - function rasterizer stage
		vk::PipelineRasterizationStateCreateInfo m_rasterizer_state_info;

		// control how the pipeline goes about blending into some given attachment.
		vk::PipelineColorBlendAttachmentState m_color_blend_state_attachment;

		// setup for depth and stencil buffer
		vk::PipelineDepthStencilStateCreateInfo m_depth_stencil_state_info;

		// configure MSAA for a pipeline.
		vk::PipelineMultisampleStateCreateInfo m_multisample_state_info;

		// additional vulkan struct : info of the shader inputs (push constants and descriptor sets) of a pipeline.
		vk::PipelineLayout m_pipeline_layout;
	};
}