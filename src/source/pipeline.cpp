#include "../include/pipeline.h"

#include <iostream>

namespace halo
{
	vk::Pipeline PipelineBuilder::create_pipeline(vk::Device device, vk::RenderPass render_pass)
	{
		vk::PipelineViewportStateCreateInfo viewport_state_create_info = {};
		viewport_state_create_info.viewportCount = 1;
		viewport_state_create_info.pViewports = &m_viewport;

		viewport_state_create_info.scissorCount = 1;
		viewport_state_create_info.pScissors = &m_scissor;

		// Dummy color blending
		vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
		color_blend_state_create_info.logicOpEnable = false;
		color_blend_state_create_info.logicOp = vk::LogicOp::eCopy;
		color_blend_state_create_info.attachmentCount = 1;
		color_blend_state_create_info.pAttachments = &m_color_blend_state_attachment;
	
		// combine all the structs ito pipeline

		vk::GraphicsPipelineCreateInfo pipeline_create_info = {};
		pipeline_create_info.stageCount = m_shader_stages.size();
		pipeline_create_info.pStages = m_shader_stages.data();

		pipeline_create_info.pVertexInputState = &m_vertex_input_info;
		pipeline_create_info.pInputAssemblyState = &m_input_assembler;

		pipeline_create_info.pViewportState = &viewport_state_create_info;
		
		pipeline_create_info.pRasterizationState = &m_rasterizer_state_info;
		pipeline_create_info.pMultisampleState = &m_multisample_state_info;
		pipeline_create_info.pColorBlendState = &color_blend_state_create_info;

		pipeline_create_info.layout = m_pipeline_layout;
		pipeline_create_info.renderPass = render_pass;
		pipeline_create_info.subpass = 0;
		
		vk::ResultValue<vk::Pipeline> pipeline = device.createGraphicsPipeline(nullptr, pipeline_create_info);
		return pipeline.value;
	}
}