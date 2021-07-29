#ifndef VK_INITIALIZERS_H
#define VK_INITIALIZERS_H

#include "../../../../third_party/vk_bootstrap.h"

#include <vulkan/vulkan.h>

//Namespace for having utility functions to create vulkan objects.
//TODO : Naming convention is pretty bad, but not to be concerned with it now.
namespace halogen::vkinit
{
    namespace command_objects
    {
        VkCommandPoolCreateInfo create_command_pool_create_info(uint32_t queue_family_index = -1, VkCommandPoolCreateFlags flags = 0);
        VkCommandBufferAllocateInfo create_command_buffer_allocate_info(VkCommandPool command_pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    }

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
        VkPipelineLayoutCreateInfo create_pipeline_layout_create_info();
    }
}

#endif