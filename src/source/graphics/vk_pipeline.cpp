#include "../../include/core/graphics/vk_pipeline.h"

namespace halogen
{
    void pipeline_utils::create_shader_module(VkDevice device, const char *shader_file_path, VkShaderModule *shader_module)
    {
        std::vector<char> shader_code;
        shader_code = common::read_binary_file_from_path(shader_file_path);

        VkShaderModuleCreateInfo shader_module_create_info = {};
        shader_module_create_info = vkinit::pipeline_objects::create_shader_module_create_info(shader_code);

        halogen::ASSERT(shader_module == nullptr, "Passed in a invalid shader module");

        vk_check(vkCreateShaderModule(device, &shader_module_create_info, nullptr, shader_module), "Failed to create shader module");
    }

    //Acually build the pipeline by connecting all the previous stuff into one place.
    VkPipeline PipelineConfig::build_pipeline(VkDevice device, VkRenderPass render_pass)
    {
        VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
        viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_create_info.pNext = nullptr;

        viewport_state_create_info.viewportCount = 1;
        viewport_state_create_info.pViewports = &m_viewport;
        viewport_state_create_info.scissorCount = 1;
        viewport_state_create_info.pScissors = &m_scissor_rectangle;

        //Dummy color blending. Everything is basically disabled for now.
        //Writing to the color attachment is possible, but no blending for now.

        //In general stores information about the attachments used (match it with whatever is the output of the fragment shader).
        VkPipelineColorBlendStateCreateInfo color_blending = {};
        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.pNext = nullptr;
        color_blending.logicOp = VK_LOGIC_OP_COPY;
        color_blending.logicOpEnable = VK_FALSE;
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &m_color_blend_attachment;

        //Build the pipeline.
        VkGraphicsPipelineCreateInfo pipeline_create_info = {};
        pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_create_info.pNext = nullptr;
        pipeline_create_info.stageCount = m_shader_stages.size();
        pipeline_create_info.pStages = m_shader_stages.data();
        pipeline_create_info.pVertexInputState = &m_vertex_input_stage_info;
        pipeline_create_info.pInputAssemblyState = &m_input_assembly_state_info;
        pipeline_create_info.pViewportState = &viewport_state_create_info;
        pipeline_create_info.pRasterizationState = &m_rasterization_state_create_info;
        pipeline_create_info.pMultisampleState = &m_multisample_state_create_info;
        pipeline_create_info.pColorBlendState = &color_blending;
        pipeline_create_info.layout = m_layout;
        pipeline_create_info.renderPass = render_pass;
        pipeline_create_info.subpass = 0;
        pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

        VkPipeline pipeline;
        vk_check(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline), "Failed to create graphics pipeline");

        return pipeline;
    }
}