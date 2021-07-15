#ifndef PIPELINE_H
#define PIPELINE_H

#include "vk_debug.h"
#include "../../log.h"
#include "../../common.h"
#include "renderer_settings.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace halogen
{
    /* Config struct for pipeline : can be used to set up multiple pipelines later. */
    struct PipelineConfiguration
    {
        std::vector<VkPipelineShaderStageCreateInfo> shader_stages_info;
        VkPipelineVertexInputStateCreateInfo vertex_input_state_info;
        VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info;
        VkViewport viewport;
        VkRect2D scissor_rectangle;
        VkPipelineViewportStateCreateInfo viewport_state_info;
        VkPipelineRasterizationStateCreateInfo rasterization_state_info;
        VkPipelineMultisampleStateCreateInfo mulisample_state_info;
        VkPipelineColorBlendAttachmentState color_blend_attachment_state;
        VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
        std::vector<VkDynamicState> dynamic_states;
        VkPipelineDynamicStateCreateInfo dynamic_state_info;
    };

    /* Class for the graphics pipeline that binds all programmable and fixed function pipeline stages from the pipeline config struct. */
    class Pipeline
    {
    public:
        Pipeline(VkInstance instance, VkDevice device);

        void create_graphics_pipeline();

        ~Pipeline();

    private:
        VkShaderModule create_shader_module(const char *shader_file_path);

    private:
        PipelineConfiguration m_pipeline_config;

        VkPipelineLayout m_pipeline_layout;
        VkPipeline m_pipeline;

        VkInstance m_vulkan_instance;
        VkDevice m_device;
    };
}
#endif