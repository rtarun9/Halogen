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

    VkImageCreateInfo image_objects::create_image_create_info(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent)
	{
		VkImageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.imageType = VK_IMAGE_TYPE_2D;
		create_info.format = format;

		//Size of image in pixels.
		create_info.extent = extent;
		create_info.mipLevels = 1;
		create_info.arrayLayers = 1;

		//No MSAA
		create_info.samples = VK_SAMPLE_COUNT_1_BIT;

		//Tiling describes how data for texture is arranged in GPU. Using VK_IMAGE_TILING_OPTIMAL lets driver decide how
		//GPU arranges memory of image
		//[NOTE : ] By using TILING_OPTIMAL you cant read data from CPU without changing tiling mode first.
		create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		create_info.usage = usage_flags;

		return create_info;
	}

	VkImageViewCreateInfo image_objects::create_image_view_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.image = image;
		create_info.format = format;

		//Information about where image point to : used because image view wrap an image, and its necessary to point to the original one.
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;

		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		create_info.subresourceRange.aspectMask = aspect_flags;

		return create_info;
	}

	//Functions related to pipeline and pipeline layout
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

		//If using a triangle list, it will consider every set of 3 vertices to be a vertex of a triangle. If using a triangle strip, the second triangle will use last 2 vertices of previous triangle + vertex at index 3.
		create_info.topology = topology;

		//If the topology is of a strip varient, you can break up the strip using a special index (0XFFF or 0XFFFFFFFFF). Not going to be used now since using strips limits the geometry that can be drawn.
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

		//Clamps the pixel's z value if z value of gl_Position is not in range [0, 1]. Not needed for now.
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
		//Note : no MSAA (Multisample state anti aliasing) for now. sample count of 1 is the default.
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

	VkPipelineDepthStencilStateCreateInfo pipeline_objects::create_depth_stencil_state_create_info(bool enable_depth_test, bool depth_write, VkCompareOp compare_op)
	{
		VkPipelineDepthStencilStateCreateInfo create_info = {};
		create_info.sType =  VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		create_info.pNext = nullptr;

		create_info.depthTestEnable = enable_depth_test ? VK_TRUE : VK_FALSE;
		create_info.depthWriteEnable = depth_write ? VK_TRUE : VK_FALSE;
		create_info.depthCompareOp = compare_op;
		create_info.depthBoundsTestEnable = VK_FALSE;
		create_info.stencilTestEnable = VK_FALSE;

		return create_info;
	}

	//Pipeline layout needs to be created seperately from the actual pipeline (should probably move this somewhere else).
	//Contains information of the different shader inputs, and usually used to configure push constants and such. (Layout of input data that pipeline needs)
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

		//Efficient way to send data to shader programs.
		create_info.pPushConstantRanges = nullptr;

		return create_info;
	}

	AllocatedBuffer buffers::create_buffer(size_t buffer_size, VkBufferUsageFlagBits usage_flags, VmaMemoryUsage memory_usage, VmaAllocator allocator)
	{
    	AllocatedBuffer buffer = {};

    	VkBufferCreateInfo buffer_create_info = {};
    	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    	buffer_create_info.pNext = nullptr;
    	buffer_create_info.size = buffer_size;
    	buffer_create_info.usage = usage_flags;

    	VmaAllocationCreateInfo allocation_create_info = {};
    	allocation_create_info.usage = memory_usage;

    	vmaCreateBuffer(allocator, &buffer_create_info, &allocation_create_info, &buffer.m_buffer, &buffer.m_allocation, nullptr);

    	return buffer;
	}
}