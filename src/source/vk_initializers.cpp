#include "../include/vk_initializers.h"

namespace halo::init
{
	VkCommandPoolCreateInfo create_command_pool(uint32_t queue_family_index, VkCommandPoolCreateFlags flags)
	{
		VkCommandPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = flags;
		create_info.queueFamilyIndex = queue_family_index;
		
		return create_info;
	}

	VkCommandBufferAllocateInfo init::create_command_buffer_allocate(VkCommandPool command_pool, VkCommandBufferLevel level)
	{
		VkCommandBufferAllocateInfo allocate_info = {};
		allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocate_info.pNext = nullptr;
		allocate_info.commandBufferCount = 1;
		allocate_info.commandPool = command_pool;
		allocate_info.level = level;

		return allocate_info;
	}

	VkPipelineShaderStageCreateInfo create_shader_stage(VkShaderStageFlagBits shader_stage, VkShaderModule shader_module)
	{
		VkPipelineShaderStageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.stage = shader_stage;
		create_info.module = shader_module;
		create_info.pName = "main";
		create_info.pSpecializationInfo = 0;

		return create_info;
	}

	VkPipelineVertexInputStateCreateInfo create_vertex_input_state()
	{
		VkPipelineVertexInputStateCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		create_info.pNext = nullptr;

		create_info.vertexAttributeDescriptionCount = 0;
		create_info.vertexBindingDescriptionCount = 0;

		return create_info;
	}

	VkPipelineInputAssemblyStateCreateInfo create_input_assembler()
	{
		VkPipelineInputAssemblyStateCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		create_info.primitiveRestartEnable = VK_FALSE;
		
		return create_info;
	}

	VkPipelineRasterizationStateCreateInfo create_rasterizer_state()
	{
		VkPipelineRasterizationStateCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.depthClampEnable = VK_FALSE;
		create_info.rasterizerDiscardEnable = VK_FALSE;
		create_info.polygonMode = VK_POLYGON_MODE_FILL;
		create_info.lineWidth = 1.0f;
		create_info.cullMode =  VK_CULL_MODE_NONE;
		create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		create_info.depthBiasEnable = VK_FALSE;
		create_info.depthBiasConstantFactor = 0.0f;
		create_info.depthBiasClamp  = 0.0f;
		create_info.depthBiasSlopeFactor = 0.0f;

		return create_info;
	}

	VkPipelineMultisampleStateCreateInfo create_multisampling_info()
	{
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

	VkPipelineColorBlendAttachmentState create_color_blend_state()
	{
		VkPipelineColorBlendAttachmentState attachment_state = {};
		attachment_state.colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT;
		attachment_state.blendEnable = VK_FALSE;

		return attachment_state;
	}

	VkPipelineLayoutCreateInfo create_pipeline_layout()
	{
		VkPipelineLayoutCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		create_info.pNext = nullptr;

		create_info.flags = 0;
		create_info.setLayoutCount = 0;
		create_info.pSetLayouts = nullptr;
		create_info.pPushConstantRanges = nullptr;
		create_info.pushConstantRangeCount = 0;

		return create_info;
	}
}