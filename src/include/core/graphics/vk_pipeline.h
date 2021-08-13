#pragma once

#include "../log.h"
#include "../common.h"
#include "vk_initializers.h"
#include "vk_debug.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace halogen
{
    //struct for base pipeline configuration.
    //Bound inside command buffers. Anything that we draw will use the pipeline in this way.
    struct PipelineConfig
    {
    public:
        std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;
        VkPipelineVertexInputStateCreateInfo m_vertex_input_stage_info;
        VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state_info;
        VkViewport m_viewport;
        VkRect2D m_scissor_rectangle;
        VkPipelineColorBlendAttachmentState m_color_blend_attachment;
        VkPipelineRasterizationStateCreateInfo m_rasterization_state_create_info;
        VkPipelineMultisampleStateCreateInfo m_multisample_state_create_info;
        VkPipelineDepthStencilStateCreateInfo m_depth_stencil_state_create_info;
        VkPipelineLayout m_layout;

        VkPipeline build_pipeline(VkDevice device, VkRenderPass render_pass);
    };

    namespace pipeline_utils
    {
        void create_shader_module(VkDevice device, const char *shader_file_path, VkShaderModule *shader_module);
    };

    //Similar to command_objects, but for pipelines.
    namespace pipeline_objects
    {
        VkShaderModuleCreateInfo create_shader_module_create_info(const std::vector<char>& code);
        VkPipelineShaderStageCreateInfo create_pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shader_module);

        [[maybe_unused]]
        VkPipelineVertexInputStateCreateInfo create_vertex_input_state_create_info();

        VkPipelineInputAssemblyStateCreateInfo create_input_assembly_state_create_info(VkPrimitiveTopology topology);
        VkPipelineRasterizationStateCreateInfo create_rasterization_state_create_info(VkPolygonMode polygon_mode);
        VkPipelineMultisampleStateCreateInfo create_multisample_state_create_info();
        VkPipelineColorBlendAttachmentState create_color_blend_attachment_state();
		VkPipelineDepthStencilStateCreateInfo create_depth_stencil_state_create_info(bool enable_depth_test, bool depth_write, VkCompareOp compare_op);
        VkPipelineLayoutCreateInfo create_pipeline_layout_create_info();
    }
}
