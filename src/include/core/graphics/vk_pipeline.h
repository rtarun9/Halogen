#ifndef VK_TYPES_H
#define VK_TYPES_H

#include "../../log.h"
#include "../../common.h"
#include "vk_initializers.h"
#include "vk_debug.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace halogen
{
    struct PipelineConfig
    {
        std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;
        VkPipelineVertexInputStateCreateInfo m_vertex_input_stage_info;
        VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state_info;
        VkViewport m_viewport;
        VkRect2D m_scissor_rectangle;
        VkPipelineColorBlendAttachmentState m_color_blend_attachment;
        VkPipelineRasterizationStateCreateInfo m_rasterization_state_create_info;
        VkPipelineMultisampleStateCreateInfo m_multisample_state_create_info;
        VkPipelineLayout m_layout;
    };

    struct Pipeline
    {
        void create_shader_module(VkDevice device, const char *shader_file_path, VkShaderModule *shader_module);
        VkPipeline build_pipeline(VkDevice device, VkRenderPass render_pass, PipelineConfig pipeline_config);
        VkPipeline m_pipeline;
        VkPipelineLayout m_layout;
    };
}

#endif