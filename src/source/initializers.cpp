#include "../include/initializers.h"

namespace halo::init
{
	vk::CommandPoolCreateInfo create_command_pool(uint32_t queue_family_index, vk::CommandPoolCreateFlags flags)
	{
		vk::CommandPoolCreateInfo create_info = {};
		create_info.flags = flags;
		create_info.queueFamilyIndex = queue_family_index;
		create_info.pNext = nullptr;

		return create_info;
	}

	vk::CommandBufferAllocateInfo create_command_buffer_allocate(vk::CommandPool command_pool, vk::CommandBufferLevel level)
	{
		vk::CommandBufferAllocateInfo allocate_info = {};
		allocate_info.commandPool = command_pool;
		allocate_info.level = level;
		allocate_info.commandBufferCount = 1;

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

		// set color write mask to RGBA
		attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
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

	VkPipelineDepthStencilStateCreateInfo create_depth_stencil_state()
	{
		VkPipelineDepthStencilStateCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		create_info.pNext = nullptr;
		
		create_info.depthTestEnable = VK_TRUE;
		create_info.depthWriteEnable = VK_TRUE;
		create_info.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		create_info.depthBoundsTestEnable = VK_FALSE;
		create_info.minDepthBounds = 0.0f;
		create_info.maxDepthBounds = 1.0f;
		create_info.stencilTestEnable = VK_FALSE;

		return create_info;
	}

	VkImageCreateInfo create_image_info(VkFormat format, VkExtent3D extent, VkImageUsageFlags usage)
	{
		VkImageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;

		create_info.imageType = VK_IMAGE_TYPE_2D;
		create_info.format = format;
		create_info.extent = extent;
		
		create_info.mipLevels = 1;
		create_info.arrayLayers = 1;

		create_info.samples = VK_SAMPLE_COUNT_1_BIT;

		// VK_IMAGE_TILING_OPTIMAL specifies optimal tiling (since texels are laid out in a implementation dependent way, it cannot be read from CPU).
		create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		
		create_info.usage = usage;
		
		//create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		return create_info;
	}

	VkImageViewCreateInfo create_image_view_info(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.pNext = nullptr;

		create_info.format = format;
		create_info.image = image;
		create_info.flags = 0;

		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

		//subresourceRange holds the information about where the image points to. 
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.layerCount = 1;
		create_info.subresourceRange.aspectMask = aspect_flags;

		return create_info;
	}
}