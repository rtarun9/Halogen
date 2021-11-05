#pragma once

#include "types.h"

#include <vector>

namespace halo
{
	class PipelineBuilder
	{
	public:
		[[nodiscard]]
		VkPipeline create_pipeline(VkDevice device, VkRenderPass renderpass);

	public:

		// contains all shader stages (vertex, fragment, compute, etc)
		std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;

		// contains info of the vertex buffer and its formats (specify how data is laid out and how to interpret it).
		VkPipelineVertexInputStateCreateInfo m_vertex_input_info;
		
		// contains info of the topology to be used while drawing
		VkPipelineInputAssemblyStateCreateInfo m_input_assembler;
		
		VkViewport m_viewport;
		VkRect2D m_scissor;

		// configuration of fixed - function rasterizer stage
		VkPipelineRasterizationStateCreateInfo m_rasterizer_state_info;

		// configure MSAA for a pipeline.
		VkPipelineMultisampleStateCreateInfo m_multisample_state_info;

		// control how th pipeline goes about blending into some given attachment.
		VkPipelineColorBlendAttachmentState m_color_blend_state_attachment;

		// hold information about depth and stencil states
		VkPipelineDepthStencilStateCreateInfo m_depth_stencil_state_info;

		// additional vulkan struct : info of the shader inputs (all types) of a pipeline.
		VkPipelineLayout m_pipeline_layout;
	};
}