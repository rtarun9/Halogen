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

	vk::PipelineDepthStencilStateCreateInfo  create_depth_stencil_state()
	{
		vk::PipelineDepthStencilStateCreateInfo  create_info = {};
	
		create_info.depthTestEnable = true;
		create_info.depthWriteEnable = true;
		create_info.depthCompareOp = vk::CompareOp::eLessOrEqual;
		create_info.depthBoundsTestEnable = false;
		create_info.minDepthBounds = 0.0f;
		create_info.maxDepthBounds = 1.0f;
		create_info.stencilTestEnable = false;

		return create_info;
	}

	vk::ImageCreateInfo create_image_info(vk::Format format, vk::Extent3D extent, vk::ImageUsageFlagBits usage)
	{
		vk::ImageCreateInfo  create_info = {};

		create_info.imageType = vk::ImageType::e2D;
		create_info.format = format;
		create_info.extent = extent;
		
		create_info.mipLevels = 1;
		create_info.arrayLayers = 1;

		create_info.samples = vk::SampleCountFlagBits::e1;

		// vk::ImageTiling::eOptimal specifies optimal tiling (since texels are laid out in a implementation dependent way, it cannot be read from CPU).
		create_info.tiling = vk::ImageTiling::eOptimal;
		
		create_info.usage = usage;
		
		return create_info;
	}

	vk::ImageViewCreateInfo create_image_view_info(vk::Format format, vk::Image image, vk::ImageAspectFlagBits aspect_flags)
	{
		vk::ImageViewCreateInfo  create_info = {};

		create_info.format = format;
		create_info.image = image;

		create_info.viewType = vk::ImageViewType::e2D;

		//subresourceRange holds the information about where the image points to. 
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.layerCount = 1;
		create_info.subresourceRange.aspectMask = aspect_flags;

		return create_info;
	}

	vk::DescriptorSetLayoutBinding create_descriptor_set_layout_binding(vk::DescriptorType type, vk::ShaderStageFlags shader_stage, uint32_t binding)
	{
		vk::DescriptorSetLayoutBinding set_layout_binding{};
		set_layout_binding.binding = binding;
		set_layout_binding.descriptorCount = 1;
		set_layout_binding.descriptorType = type;
		set_layout_binding.stageFlags = shader_stage;

		return set_layout_binding;
	}

	vk::WriteDescriptorSet write_descriptor_buffer(vk::DescriptorType type, vk::DescriptorSet descriptor_set, vk::DescriptorBufferInfo* buffer_info, uint32_t binding)
	{
		vk::WriteDescriptorSet descriptor_set_write{};
		descriptor_set_write.dstBinding = binding;
		descriptor_set_write.dstSet = descriptor_set;
		descriptor_set_write.descriptorCount = 1;
		descriptor_set_write.descriptorType = type;
		descriptor_set_write.pBufferInfo = buffer_info;

		return descriptor_set_write;
	}
}