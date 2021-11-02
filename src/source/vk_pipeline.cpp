#include "../include/vk_pipeline.h"

#include <iostream>

namespace halo
{
	VkPipeline PipelineBuilder::create_pipeline(VkDevice device, VkRenderPass renderpass)
	{
		VkPipelineViewportStateCreateInfo viewport_state_info = {};
		viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state_info.pNext = nullptr;

		viewport_state_info.viewportCount = 1;
		viewport_state_info.pViewports = &m_viewport;

		viewport_state_info.scissorCount = 1;
		viewport_state_info.pScissors = &m_scissor;

		// dummy color blending - blending is "none.
		VkPipelineColorBlendStateCreateInfo color_blend_state = {};
		color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blend_state.pNext = nullptr;

		color_blend_state.logicOpEnable = VK_FALSE;
		color_blend_state.logicOp = VK_LOGIC_OP_COPY;
		color_blend_state.attachmentCount = 1;
		color_blend_state.pAttachments = &m_color_blend_state_attachment;

		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.pNext = nullptr;

		pipeline_info.stageCount = m_shader_stages.size();
		pipeline_info.pStages = m_shader_stages.data();

		pipeline_info.pVertexInputState = &m_vertex_input_info;
		pipeline_info.pInputAssemblyState = &m_input_assembler;
		pipeline_info.pViewportState = &viewport_state_info;
		pipeline_info.pRasterizationState = &m_rasterizer_state_info;
		pipeline_info.pMultisampleState = &m_multisample_state_info;
		pipeline_info.pColorBlendState = &color_blend_state;
		pipeline_info.layout = m_pipeline_layout;
		pipeline_info.renderPass = renderpass;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;


		VkPipeline new_pipeline;
		VkResult res = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &new_pipeline);
		if (res != VK_SUCCESS)
		{
			std::cout << "Failed to create pipeline";
			return VK_NULL_HANDLE;
		}

		return new_pipeline;
	}
}