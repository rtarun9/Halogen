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

	vk::FenceCreateInfo create_fence(vk::FenceCreateFlags flags)
	{
		vk::FenceCreateInfo create_info = {};
		create_info.flags = flags;

		return create_info;
	}

	vk::SemaphoreCreateInfo create_semaphore()
	{
		vk::SemaphoreCreateInfo create_info = {};

		return create_info;
	}

	vk::PipelineShaderStageCreateInfo create_shader_stage(vk::ShaderStageFlagBits shader_stage, vk::ShaderModule shader_module)
	{
		vk::PipelineShaderStageCreateInfo create_info = {};
		
		create_info.stage = shader_stage;
		create_info.module = shader_module;
		create_info.pName = "main";
		create_info.pSpecializationInfo = 0;

		return create_info;
	}

	vk::PipelineVertexInputStateCreateInfo create_vertex_input_state()
	{
		vk::PipelineVertexInputStateCreateInfo  create_info = {};

		create_info.vertexAttributeDescriptionCount = 0;
		create_info.vertexBindingDescriptionCount = 0;

		return create_info;
	}

	vk::PipelineInputAssemblyStateCreateInfo create_input_assembler(vk::PrimitiveTopology topology)
	{
		vk::PipelineInputAssemblyStateCreateInfo create_info = {};
		
		create_info.topology = topology;
		create_info.primitiveRestartEnable = VK_FALSE;
		
		return create_info;
	}

	vk::PipelineRasterizationStateCreateInfo create_rasterizer_state()
	{
		vk::PipelineRasterizationStateCreateInfo create_info = {};

		create_info.depthClampEnable = false;

		create_info.rasterizerDiscardEnable = false;
		create_info.polygonMode = vk::PolygonMode::eFill;
		create_info.lineWidth = 1.0f;
		create_info.cullMode =  vk::CullModeFlagBits::eNone;
		create_info.frontFace = vk::FrontFace::eClockwise;
		create_info.depthBiasEnable = false;
		create_info.depthBiasConstantFactor = 0.0f;
		create_info.depthBiasClamp  = 0.0f;
		create_info.depthBiasSlopeFactor = 0.0f;

		return create_info;
	}

	vk::PipelineMultisampleStateCreateInfo create_multisampling_info()
	{
		vk::PipelineMultisampleStateCreateInfo create_info = {};

		create_info.sampleShadingEnable = false;
		create_info.rasterizationSamples = vk::SampleCountFlagBits::e1;
		create_info.minSampleShading = 1.0f;
		create_info.pSampleMask = nullptr;
		create_info.alphaToCoverageEnable = false;
		create_info.alphaToOneEnable = false;

		return create_info;
	}

	vk::PipelineColorBlendAttachmentState create_color_blend_state()
	{
		vk::PipelineColorBlendAttachmentState attachment_state = {};

		// set color write mask to RGBA
		attachment_state.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		attachment_state.blendEnable = false;

		return attachment_state;
	}

	vk::PipelineLayoutCreateInfo create_pipeline_layout()
	{
		vk::PipelineLayoutCreateInfo create_info = {};
	
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