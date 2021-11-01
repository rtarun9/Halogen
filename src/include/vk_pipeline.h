#pragma once

#include "vk_types.h"

#include <vector>

namespace halo
{
	class PipelineBuilder
	{
	public:
		VkPipeline create_pipeline(VkDevice device, VkRenderPass renderpass);

	public:
		std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;

		VkPipelineVertexInputStateCreateInfo m_vertex_input_info;
		VkPipelineInputAssemblyStateCreateInfo m_input_assembler;
		
		VkViewport m_viewport;
		VkRect2D m_scissor;

		VkPipelineRasterizationStateCreateInfo m_rasterizer_state_info;
		VkPipelineColorBlendAttachmentState m_color_blend_state_attachment;
		VkPipelineMultisampleStateCreateInfo m_multisample_state_info;

		VkPipelineLayout m_pipeline_layout;
	};
}