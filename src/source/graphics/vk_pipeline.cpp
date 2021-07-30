#include "../../include/core/graphics/vk_pipeline.h"

namespace halogen
{
    void pipeline_utils::create_shader_module(VkDevice device, const char *shader_file_path, VkShaderModule *shader_module)
    {
        std::vector<char> shader_code;
        shader_code = read_binary_file_from_path(shader_file_path);

        VkShaderModuleCreateInfo shader_module_create_info = {};
        shader_module_create_info = pipeline_objects::create_shader_module_create_info(shader_code);

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


    VkShaderModuleCreateInfo pipeline_objects::create_shader_module_create_info(const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.pNext = nullptr;
        create_info.codeSize = code.size();

        //codeSize is in bytes, but pCode needs to be a uint32_t pointer. So, using reinterpret cast here.
        //std::vector guarentees data satisfies worst case alignment requirements, so this should be fine.
        create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

        return create_info;
    }

    //Holds information about single shader stage.
    VkPipelineShaderStageCreateInfo pipeline_objects::create_pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shader_module)
    {
        VkPipelineShaderStageCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        create_info.pNext = nullptr;
        create_info.stage = stage;

        //Entry point for the shader stage can anything, sticking with main for simplicity for now.
        create_info.pName = "main";

        create_info.module = shader_module;
        return create_info;
    }

    //Holds information for vertex buffers and other vertex formats. (Basically the VAO struct in openGL).
    //Unused for now.
    VkPipelineVertexInputStateCreateInfo pipeline_objects::create_vertex_input_state_create_info()
    {
        VkPipelineVertexInputStateCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        create_info.pNext = nullptr;
        create_info.vertexAttributeDescriptionCount = 0;
        create_info.vertexBindingDescriptionCount = 0;

        return create_info;
    }

    //Holds information of the type of topology to be drawn. Set pipeline to either draw triangles, lines, points, etc.
    VkPipelineInputAssemblyStateCreateInfo pipeline_objects::create_input_assembly_state_create_info(VkPrimitiveTopology topology)
    {
        VkPipelineInputAssemblyStateCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        create_info.pNext = nullptr;
        create_info.topology = topology;

        //Yeah no clue what this means, so ignoring for now
        create_info.primitiveRestartEnable = VK_FALSE;

        return create_info;
    }

    //Config for fixed function rasterization.
    //Enable / Disable backface culling, line width, and wireframe drawing.
    VkPipelineRasterizationStateCreateInfo pipeline_objects::create_rasterization_state_create_info(VkPolygonMode polygon_mode)
    {
        VkPipelineRasterizationStateCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        create_info.pNext = nullptr;
        create_info.depthClampEnable = VK_FALSE;

        //Discard all primitives before the rasterization stage.
        //Basically means triangles and other primitives will never be drawn onto the screen.
        //Only used in rare cases (in observing side effects of vertex processing stages).

        create_info.rasterizerDiscardEnable = VK_FALSE;
        create_info.polygonMode = polygon_mode;
        create_info.lineWidth = 1.0f;

        //Disable backface culling for now
        create_info.cullMode = VK_CULL_MODE_NONE;
        create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;

        //Disable anything related to depth biasing for now.
        create_info.depthBiasClamp = 0.0f;
        create_info.depthBiasConstantFactor = 0.0f;
        create_info.depthBiasEnable = VK_FALSE;
        create_info.depthBiasSlopeFactor = 0.0f;

        return create_info;
    }

    //Configuration for MSAA
    VkPipelineMultisampleStateCreateInfo pipeline_objects::create_multisample_state_create_info()
    {
        //Note : no MSAA for now. sample count of 1 is the default.
        VkPipelineMultisampleStateCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        create_info.pNext = nullptr;
        create_info.sampleShadingEnable = VK_FALSE;
        create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        create_info.minSampleShading = 1.0f;
        create_info.pSampleMask = nullptr;
        create_info.alphaToCoverageEnable = VK_FALSE;
        create_info.alphaToOneEnable = VK_FALSE;

        return create_info;
    }

    //Control how the pipeline will blend given attachment.
    //Will use only one attachment for a while, so default is to disable blending.
    VkPipelineColorBlendAttachmentState pipeline_objects::create_color_blend_attachment_state()
    {
        VkPipelineColorBlendAttachmentState color_blend_attachment = {};
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT  | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_R_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        return color_blend_attachment;
    }

    //Pipeline layout needs to be created seperately from the actual pipeline (should probably move this somewhere else).
    //Contains information of the different shader inputs, and usually used to configure push constants and such.
    //Since none of those are going to be used, it is empty for now.
    VkPipelineLayoutCreateInfo pipeline_objects::create_pipeline_layout_create_info()
    {
        VkPipelineLayoutCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        create_info.pNext = nullptr;

        create_info.flags = 0;
        create_info.setLayoutCount = 0;
        create_info.pSetLayouts = nullptr;
        create_info.pushConstantRangeCount = 0;
        create_info.pPushConstantRanges = nullptr;

        return create_info;
    }
}