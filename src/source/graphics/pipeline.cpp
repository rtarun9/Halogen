#include "../../include/core/graphics/pipeline.h"

namespace halogen
{
    Pipeline::Pipeline(VkInstance instance, VkDevice device) : m_vulkan_instance(instance), m_device(device)
    {
        create_graphics_pipeline();
    }

    void Pipeline::create_graphics_pipeline()
    {
        VkShaderModule vertex_shader_module = create_shader_module("vertex_shader.spv");
        VkShaderModule fragment_shader_module = create_shader_module("fragment_shader.spv");

        VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {};
        vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertex_shader_stage_create_info.module = vertex_shader_module;
        vertex_shader_stage_create_info.pName = "main";

        VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {};
        fragment_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragment_shader_stage_create_info.module = fragment_shader_module;
        fragment_shader_stage_create_info.pName = "main";

        m_pipeline_config.shader_stages_info.emplace_back(vertex_shader_stage_create_info);
        m_pipeline_config.shader_stages_info.emplace_back(fragment_shader_stage_create_info);

        m_pipeline_config.vertex_input_state_info= {};
        m_pipeline_config.vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        m_pipeline_config.vertex_input_state_info.vertexBindingDescriptionCount = 0;
        m_pipeline_config.vertex_input_state_info.pVertexBindingDescriptions = nullptr;
        m_pipeline_config.vertex_input_state_info.vertexAttributeDescriptionCount = 0;
        m_pipeline_config.vertex_input_state_info.pVertexAttributeDescriptions = nullptr;

        m_pipeline_config.input_assembly_state_info = {};
        m_pipeline_config.input_assembly_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        m_pipeline_config.input_assembly_state_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        m_pipeline_config.input_assembly_state_info.primitiveRestartEnable = VK_FALSE;


        m_pipeline_config.viewport = {};
        m_pipeline_config.viewport.x = 0.0f;
        m_pipeline_config.viewport.y = 0.0f;
        m_pipeline_config.viewport.width = static_cast<float>(settings::SwapchainSettings::instance().surface_extent.width);
        m_pipeline_config.viewport.height = static_cast<float>(settings::SwapchainSettings::instance().surface_extent.height);
        m_pipeline_config.viewport.minDepth = 0.0f;
        m_pipeline_config.viewport.maxDepth = 1.0f;

        m_pipeline_config.scissor_rectangle = {};
        m_pipeline_config.scissor_rectangle.offset = {0, 0};
        m_pipeline_config.scissor_rectangle.extent = settings::SwapchainSettings::instance().surface_extent;


        m_pipeline_config.viewport_state_info = {};
        m_pipeline_config.viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        m_pipeline_config.viewport_state_info.viewportCount = 1;
        m_pipeline_config.viewport_state_info.pViewports = &m_pipeline_config.viewport;
        m_pipeline_config.viewport_state_info.scissorCount = 1;
        m_pipeline_config.viewport_state_info.pScissors = &m_pipeline_config.scissor_rectangle;

        m_pipeline_config.rasterization_state_info = {};
        m_pipeline_config.rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        m_pipeline_config.rasterization_state_info.depthClampEnable = VK_FALSE;
        m_pipeline_config.rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
        m_pipeline_config.rasterization_state_info.polygonMode = VK_POLYGON_MODE_FILL;
        m_pipeline_config.rasterization_state_info.lineWidth = 1.0f;
        m_pipeline_config.rasterization_state_info.cullMode = VK_CULL_MODE_BACK_BIT;
        m_pipeline_config.rasterization_state_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        m_pipeline_config.rasterization_state_info.depthBiasClamp = VK_FALSE;

        m_pipeline_config.mulisample_state_info = {};
        m_pipeline_config.mulisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        m_pipeline_config.mulisample_state_info.sampleShadingEnable = VK_FALSE;
        m_pipeline_config.mulisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        m_pipeline_config.color_blend_attachment_state = {};
        m_pipeline_config.color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        m_pipeline_config.color_blend_attachment_state.blendEnable = VK_FALSE;

        m_pipeline_config.color_blend_state_create_info = {};
        m_pipeline_config.color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        m_pipeline_config.color_blend_state_create_info.logicOpEnable = VK_FALSE;
        m_pipeline_config.color_blend_state_create_info.attachmentCount = 1;
        m_pipeline_config.color_blend_state_create_info.pAttachments = &m_pipeline_config.color_blend_attachment_state;

        m_pipeline_config.dynamic_states.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
        m_pipeline_config.dynamic_states.emplace_back(VK_DYNAMIC_STATE_LINE_WIDTH);

        m_pipeline_config.dynamic_state_info = {};
        m_pipeline_config.dynamic_state_info.dynamicStateCount = m_pipeline_config.dynamic_states.size();
        m_pipeline_config.dynamic_state_info.pDynamicStates = m_pipeline_config.dynamic_states.data();

        VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
        pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        VK_CHECK(vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &m_pipeline_layout));

        vkDestroyShaderModule(m_device, vertex_shader_module, nullptr);
        vkDestroyShaderModule(m_device, fragment_shader_module, nullptr);
    }

    Pipeline::~Pipeline()
    {
        vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
        //vkDestroyPipeline(m_device, m_pipeline, nullptr);
    }

    VkShaderModule Pipeline::create_shader_module(const char *shader_file_path)
    {
        std::vector<char> shader_source_code = common::read_binary_file_from_path(shader_file_path);
        ASSERT(shader_source_code.empty(), "Cannot create shader since the SPIR-V code is empty.");

        VkShaderModuleCreateInfo shader_module_create_info = {};
        shader_module_create_info = {};
        shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_info.flags = 0;
        shader_module_create_info.codeSize = shader_source_code.size();
        shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(shader_source_code.data());

        VkShaderModule shader_module;
        VK_CHECK(vkCreateShaderModule(m_device, &shader_module_create_info, nullptr, &shader_module));
        return shader_module;
    }
}