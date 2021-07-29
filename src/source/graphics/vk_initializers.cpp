#include "../../include/core/graphics/vk_initializers.h"

namespace halogen::vkinit
{
    VkCommandPoolCreateInfo command_objects::create_command_pool_create_info(uint32_t queue_family_index, VkCommandPoolCreateFlags flags)
    {
        VkCommandPoolCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        create_info.pNext = nullptr;
        create_info.flags = flags;
        create_info.queueFamilyIndex = queue_family_index;

        return create_info;
    }

    VkCommandBufferAllocateInfo command_objects::create_command_buffer_allocate_info(VkCommandPool command_pool, uint32_t count, VkCommandBufferLevel level)
    {
        VkCommandBufferAllocateInfo allocate_info = {};
        allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.pNext = nullptr;
        allocate_info.commandPool = command_pool;
        allocate_info.commandBufferCount = count;
        allocate_info.level = level;
        return allocate_info;
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