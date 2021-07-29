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
    //struct for PipelineConfig (abstracted since the pipeline is a very very big object in vulkan, since it encomposes the entire state of the GPU for the draw).
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
        VkPipelineLayout m_layout;

        //Not sure if this should be implemented in the vk_pipeline.cpp or vk_initializers.cpp file.
        VkPipeline build_pipeline(VkDevice device, VkRenderPass render_pass);
    };

    //Just some utility functions to help in creation of shader module. Probably move to the vk_initializers file at some point.
    namespace pipeline_utils
    {
        void create_shader_module(VkDevice device, const char *shader_file_path, VkShaderModule *shader_module);
    };
}

#endif