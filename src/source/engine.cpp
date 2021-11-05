#include "../include/engine.h"

#include "../include/types.h"
#include "../include/initializers.h"
#include "../include/pipeline.h"

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <glm/gtc/matrix_transform.hpp>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <fstream>

#define ONE_SECOND 1000000000
#define VK_CHECK(x) {VkResult res = x; if (res) {std::cout << "Vulkan Error : " << res << '\n'; exit(EXIT_FAILURE);}}

namespace halo
{
	Engine::Engine(const Config& config): m_config(config)
	{
		m_window_extent = {static_cast<uint32_t>(m_config.m_window_width), static_cast<uint32_t>(m_config.m_window_height)};
	}

	void Engine::initialize()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			std::cout << "Failed to initialize SDL.";
			exit(EXIT_FAILURE);
		}

		SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WindowFlags::SDL_WINDOW_VULKAN | SDL_WindowFlags::SDL_WINDOW_SHOWN);

		m_window = SDL_CreateWindow(m_config.m_window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_window_extent.width, m_window_extent.height, window_flags);
		if (m_window == nullptr)
		{
			std::cout << "Failed to create window";
			exit(EXIT_FAILURE);
		}

		initialize_vulkan();
		initialize_swapchain();
		initialize_command_objects();
		initialize_renderpass();
		initialize_framebuffers();
		initialize_synchronization_objects();
		initialize_pipeline();

		load_meshes();

		initialize_scene();

		m_is_initialized = true;
	}

	void Engine::run()
	{
		SDL_Event event;
		bool quit = false;

		while (!quit)
		{
			while (SDL_PollEvent(&event) != 0)
			{
				if (event.type == SDL_QUIT)
				{
					quit = true;
				}

				const Uint8 *keyboard_state = SDL_GetKeyboardState(nullptr);
				if (keyboard_state[SDL_SCANCODE_ESCAPE])
				{
					quit = true;
				}
			}

			render();
		}
	}

	void Engine::render()
	{
		// wait until rendering of last frame has occured. max wait time is one second (in function ns is given).
		VK_CHECK(vkWaitForFences(m_device, 1, &m_render_fence, true, ONE_SECOND));
		VK_CHECK(vkResetFences(m_device, 1, &m_render_fence));

		// we will signal the presentation semaphore when the swapchain is ready for presentation
		uint32_t swapchain_image_index = 0;
		VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, ONE_SECOND, m_present_semaphore, nullptr, &swapchain_image_index));
		
		VK_CHECK(vkResetCommandBuffer(m_command_buffer, 0));

		VkCommandBufferBeginInfo command_buffer_begin_info = {};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_begin_info.pNext = nullptr;
		command_buffer_begin_info.pInheritanceInfo = nullptr;
		command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// indicating to vulkan that this command buffer will be submitted once (since its recording everyframe, doesnt matter).
		VK_CHECK(vkBeginCommandBuffer(m_command_buffer, &command_buffer_begin_info));
		
		VkClearValue clear_value = {};
		clear_value.color = {{abs(cos(m_frame_number / 120.0f)), 0.0f, abs(sin(m_frame_number / 120.0f))}};
		clear_value.color = {{0.0f, 0.0f, 1.0f}};

		VkClearValue depth_clear_value = {};
		depth_clear_value.depthStencil.depth = 1.0f;

		VkClearValue clear_values[2] = {clear_value, depth_clear_value};

		// start renderpass
		VkRenderPassBeginInfo renderpass_begin_info = {};
		renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderpass_begin_info.pNext = nullptr;

		renderpass_begin_info.clearValueCount = 2;
		renderpass_begin_info.pClearValues = &clear_values[0];

		renderpass_begin_info.framebuffer = m_framebuffers[swapchain_image_index];
		renderpass_begin_info.renderArea.extent = m_window_extent;
		renderpass_begin_info.renderArea.offset = {0, 0};
		renderpass_begin_info.renderPass = m_renderpass;

		vkCmdBeginRenderPass(m_command_buffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
		
		draw_objects(m_command_buffer, m_game_objects.data(), static_cast<int>(m_game_objects.size()));

		vkCmdEndRenderPass(m_command_buffer);

		VK_CHECK(vkEndCommandBuffer(m_command_buffer));

		// prepare for submission to queue
		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pNext = nullptr;
		
		VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		submit_info.pWaitDstStageMask = &wait_stages;

		//we want to wait on the present Semaphore, as that semaphore is signaled when the swapchain is ready
		//we will signal the render Semaphore, to signal that rendering has finished
		
		submit_info.pWaitSemaphores = &m_present_semaphore;
		submit_info.pSignalSemaphores = &m_render_semaphore;
		submit_info.waitSemaphoreCount = 1;
		submit_info.signalSemaphoreCount = 1;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &m_command_buffer;

		// signal the render fence when rendering is complete.
		VK_CHECK(vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_render_fence));

		// presentation on screen
		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.pNext = nullptr;
		present_info.pSwapchains = &m_swapchain;
		present_info.swapchainCount = 1;
		present_info.pImageIndices = &swapchain_image_index;
		present_info.pWaitSemaphores = &m_render_semaphore;
		present_info.waitSemaphoreCount = 1;

		VK_CHECK(vkQueuePresentKHR(m_graphics_queue, &present_info));

		m_frame_number++;
	}

	void Engine::initialize_vulkan()
	{
		// use vkbootstrap for initializing core Vk objects. Will be replaced later.
		vkb::InstanceBuilder builder;

		auto instance = builder.set_app_name("Halogen")
			.request_validation_layers(true)
			.require_api_version(1, 1, 0)
			.use_default_debug_messenger()
			.build();

		vkb::Instance vkb_instance = instance.value();

		m_instance = vkb_instance.instance;
		m_debug_messenger = vkb_instance.debug_messenger;

		SDL_Vulkan_CreateSurface(m_window, m_instance, &m_surface);
	
		// physical device is selected after surface since we want to be able to render to that surface.
		// by default vkbootstrap will apparently try to choose the dedicated GPU, which is preferable.
		vkb::PhysicalDeviceSelector selector { vkb_instance };
		vkb::PhysicalDevice physical_device = selector.set_minimum_version(1, 1)
			.set_surface(m_surface)
			.select()
			.value();

		vkb::DeviceBuilder device_builder {physical_device};

		vkb::Device vkb_device = device_builder.build()
			.value();

		m_device = vkb_device.device;
		m_physical_device = physical_device.physical_device;

		m_graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
		m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
	
		// display which device was chosen
		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(m_physical_device, &device_properties);
		
		std::cout << "Device chosen : "  << device_properties.deviceName<< '\n';
		
		VmaAllocatorCreateInfo vma_allocator_create_info = {};
		vma_allocator_create_info.device = m_device;
		vma_allocator_create_info.instance = m_instance;
		vma_allocator_create_info.physicalDevice = m_physical_device;
		vmaCreateAllocator(&vma_allocator_create_info, &m_vma_allocator);
	}

	void Engine::initialize_swapchain()
	{
		// uses vkbootstrap for swapchain initialization.

		vkb::SwapchainBuilder swapchain_builder {m_physical_device, m_device, m_surface};

		// about the presentation modes : https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPresentModeKHR.html
		vkb::Swapchain vkb_swapchain = swapchain_builder	
			.use_default_format_selection()
			.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
			.set_desired_extent(m_window_extent.width, m_window_extent.height)
			.build()
			.value();

		m_swapchain = vkb_swapchain.swapchain;
		
		m_swapchain_images = vkb_swapchain.get_images().value();

		m_swapchain_image_views = vkb_swapchain.get_image_views().value();
	
		m_swapchain_image_format = vkb_swapchain.image_format;

		m_deletors.push_function([=](){vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);});
		
		VkExtent3D depth_image_extent = {};
		depth_image_extent.depth = 1;
		depth_image_extent.width = m_window_extent.width;
		depth_image_extent.height = m_window_extent.height;

		// 32 bit depth format (24 for depth, 8 for stencil)
		m_depth_format = VK_FORMAT_D32_SFLOAT;

		VkImageCreateInfo depth_image_create_info = init::create_image_info(m_depth_format, depth_image_extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

		VmaAllocationCreateInfo depth_image_allocation_info = {};
		depth_image_allocation_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		depth_image_allocation_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vmaCreateImage(m_vma_allocator, &depth_image_create_info, &depth_image_allocation_info, &m_depth_buffer.m_image, &m_depth_buffer.m_allocation_data, nullptr);

		VkImageViewCreateInfo depth_image_view_create_info = init::create_image_view_info(m_depth_format, m_depth_buffer.m_image, VK_IMAGE_ASPECT_DEPTH_BIT);
		VK_CHECK(vkCreateImageView(m_device, &depth_image_view_create_info, nullptr, &m_depth_image_view));

		m_deletors.push_function([=](){vkDestroyImageView(m_device, m_depth_image_view, nullptr);});
		m_deletors.push_function([=](){vmaDestroyImage(m_vma_allocator, m_depth_buffer.m_image, m_depth_buffer.m_allocation_data);});
	}

	void Engine::initialize_command_objects()
	{
		VkCommandPoolCreateInfo command_pool_create_info = init::create_command_pool(m_graphics_queue_family);
		VK_CHECK(vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_command_pool));
		m_deletors.push_function([=](){vkDestroyCommandPool(m_device, m_command_pool, nullptr);});

		// about life cycle of command buffer : https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap6.html#commandbuffers-lifecycle
		VkCommandBufferAllocateInfo command_buffer_allocate_info = init::create_command_buffer_allocate(m_command_pool);
		VK_CHECK(vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &m_command_buffer));
	}

	// renderpass stores the state of images rendering into, and the state needed to setup the target framebuffer for rendering.
	void Engine::initialize_renderpass()
	{
		// created before framebuffers, as the frame buffers are made for a specefic renderpass.

		// create and setup description of image we are rendering into (the color attachment we are rendering to).
		VkAttachmentDescription color_attachment_desc = {};
		color_attachment_desc.format = m_swapchain_image_format;
		color_attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment_desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

		// keep the format of attachment even after renderpass ends
		color_attachment_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		
		color_attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		color_attachment_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		// store in optimal format for presentation
		color_attachment_desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// VkAttachmenReference::attachment is the index of the attachment used  (from pAttachments in VkRenderpassCreateInfo): here it is 0 since there is only one attachment.
		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// create depth attachment
		VkAttachmentDescription depth_attachment_desc = {};
		depth_attachment_desc.flags = 0;
		depth_attachment_desc.format = m_depth_format;
		depth_attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;

		depth_attachment_desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		depth_attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
						
		depth_attachment_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment_desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// attachment reference for depth attachment
		VkAttachmentReference depth_attachment_ref = {};
		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass_desc = {};
		subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_desc.colorAttachmentCount = 1;
		subpass_desc.pColorAttachments = &color_attachment_ref;
		subpass_desc.pDepthStencilAttachment = &depth_attachment_ref;
		
		VkAttachmentDescription attachments[2] = {color_attachment_desc, depth_attachment_desc};

		VkRenderPassCreateInfo renderpass_create_info = {};
		renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpass_create_info.pNext = nullptr;
		renderpass_create_info.attachmentCount = 2;
		renderpass_create_info.pAttachments = &attachments[0];
		renderpass_create_info.subpassCount = 1;
		renderpass_create_info.pSubpasses = &subpass_desc;
		
		VK_CHECK(vkCreateRenderPass(m_device, &renderpass_create_info, nullptr, &m_renderpass));
		
		m_deletors.push_function([=](){vkDestroyRenderPass(m_device, m_renderpass, nullptr);});
	}
	
	void Engine::initialize_framebuffers()
	{
		// since most framebuffers will share a bunch of command stuff, create a common framebuffer create info with different attachments.
		VkFramebufferCreateInfo framebuffer_create_info = {};
		framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_create_info.pNext = nullptr;

		framebuffer_create_info.width = m_window_extent.width;
		framebuffer_create_info.height = m_window_extent.height;
		framebuffer_create_info.layers = 1;

		framebuffer_create_info.renderPass = m_renderpass;

		m_framebuffers.resize(m_swapchain_image_views.size());

		for (int i = 0; i < m_framebuffers.size(); i++)
		{
			VkImageView frame_buffer_attachments[2] = {m_swapchain_image_views[i], m_depth_image_view};

			framebuffer_create_info.pAttachments = frame_buffer_attachments;
			framebuffer_create_info.attachmentCount = 2;

			VK_CHECK(vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_framebuffers[i]));
			m_deletors.push_function([=](){vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);});
			m_deletors.push_function([=](){vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);});
		}
	}

	void Engine::initialize_synchronization_objects()
	{
		// the flag of fence is set ot signaled bit. This is done for the edge case (first iteration of render loop).
		VkFenceCreateInfo fence_create_info = {};
		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_create_info.pNext = nullptr;
		fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VK_CHECK(vkCreateFence(m_device, &fence_create_info, nullptr, &m_render_fence));
		m_deletors.push_function([=](){vkDestroyFence(m_device, m_render_fence, nullptr);});

		VkSemaphoreCreateInfo semaphore_create_info = {};
		semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphore_create_info.pNext = nullptr;
		semaphore_create_info.flags = 0;

		VK_CHECK(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_render_semaphore));
		VK_CHECK(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_present_semaphore));

		m_deletors.push_function([=](){vkDestroySemaphore(m_device, m_render_semaphore, nullptr);});
		m_deletors.push_function([=](){vkDestroySemaphore(m_device, m_present_semaphore, nullptr);});
	}

	void Engine::initialize_pipeline()
	{
		VkPipelineLayoutCreateInfo pipeline_layout_info = init::create_pipeline_layout();
		VK_CHECK(vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_triangle_pipeline_layout));
		m_deletors.push_function([=](){vkDestroyPipelineLayout(m_device, m_triangle_pipeline_layout, nullptr);});

		PipelineBuilder pipeline_builder;

		pipeline_builder.m_vertex_input_info = init::create_vertex_input_state();

		pipeline_builder.m_input_assembler = init::create_input_assembler();

		pipeline_builder.m_viewport.x = 0.0f;
		pipeline_builder.m_viewport.y = 0.0f;
		pipeline_builder.m_viewport.width = static_cast<float>(m_window_extent.width);
		pipeline_builder.m_viewport.height = static_cast<float>(m_window_extent.height);
		pipeline_builder.m_viewport.maxDepth = 1.0f;
		pipeline_builder.m_viewport.minDepth = 0.0f;

		pipeline_builder.m_scissor.offset = {0, 0};
		pipeline_builder.m_scissor.extent = m_window_extent;

		pipeline_builder.m_rasterizer_state_info = init::create_rasterizer_state();

		pipeline_builder.m_multisample_state_info = init::create_multisampling_info();

		pipeline_builder.m_color_blend_state_attachment = init::create_color_blend_state();

		pipeline_builder.m_depth_stencil_state_info = init::create_depth_stencil_state();

		pipeline_builder.m_pipeline_layout = m_triangle_pipeline_layout;

		m_triangle_pipeline = pipeline_builder.create_pipeline(m_device, m_renderpass);
		m_deletors.push_function([=](){vkDestroyPipeline(m_device, m_triangle_pipeline, nullptr);});

		// setup triangle "mesh" pipeline

		VkShaderModule mesh_triangle_vert;
		load_shaders("../shaders/triangle_mesh.vert.spv", mesh_triangle_vert);

		VkShaderModule mesh_triangle_frag;
		load_shaders("../shaders/simple_triangle.frag.spv", mesh_triangle_frag);

		pipeline_builder.m_shader_stages.clear();

		pipeline_builder.m_shader_stages.push_back(init::create_shader_stage(VK_SHADER_STAGE_VERTEX_BIT, mesh_triangle_vert));
		pipeline_builder.m_shader_stages.push_back(init::create_shader_stage(VK_SHADER_STAGE_FRAGMENT_BIT, mesh_triangle_frag));

		VertexInputLayoutDescription vertex_input_layout_desc = Vertex::get_vertex_input_layout_description();

		pipeline_builder.m_vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_layout_desc.m_bindings.size());
		pipeline_builder.m_vertex_input_info.pVertexBindingDescriptions = vertex_input_layout_desc.m_bindings.data();

		pipeline_builder.m_vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_layout_desc.m_attributes.size());
		pipeline_builder.m_vertex_input_info.pVertexAttributeDescriptions = vertex_input_layout_desc.m_attributes.data();

		// set up pipeline layout (now with push constants)
		VkPipelineLayoutCreateInfo mesh_pipeline_layout_create_info = init::create_pipeline_layout();

		VkPushConstantRange push_constant;
		push_constant.offset = 0;
		push_constant.size = sizeof(PushConstants);
		push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		mesh_pipeline_layout_create_info.pPushConstantRanges = &push_constant;
		mesh_pipeline_layout_create_info.pushConstantRangeCount = 1;

		VK_CHECK(vkCreatePipelineLayout(m_device, &mesh_pipeline_layout_create_info, nullptr, &m_default_pipeline_layout));

		pipeline_builder.m_pipeline_layout = m_default_pipeline_layout;

		m_deletors.push_function([=]() {vkDestroyShaderModule(m_device, mesh_triangle_vert, nullptr); });
		m_deletors.push_function([=]() {vkDestroyShaderModule(m_device, mesh_triangle_frag, nullptr); });

		m_default_pipeline = pipeline_builder.create_pipeline(m_device, m_renderpass);

		m_deletors.push_function([=](){vkDestroyPipelineLayout(m_device, m_default_pipeline_layout, nullptr);});
		m_deletors.push_function([=](){vkDestroyPipeline(m_device, m_default_pipeline, nullptr);});
	
		create_material("default_material", m_default_pipeline, m_triangle_pipeline_layout);
	}

	void Engine::load_shaders(const char* file_path, VkShaderModule& shader_module)
	{
		shader_module = {};

		// moving cursor to the end makes finding size very easy, and since .spv is in binary, the std::ios::binary flag is used.
		std::ifstream file(file_path, std::ios::ate |  std::ios::binary);

		if (!file.is_open())
		{
			std::cout << "Failed to open file with path : " << file_path;
			exit(EXIT_FAILURE);
		}

		size_t file_size = (size_t)(file.tellg());

		// technicality of spirv : file contents are expected to be in uint32
		std::vector<uint32_t> file_contents(file_size / sizeof(uint32_t));

		file.seekg(0);

		// load file contents into the buffer
		file.read((char*)file_contents.data(), file_size);

		file.close();

		VkShaderModuleCreateInfo shader_module_create_info = {};
		shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shader_module_create_info.pNext = nullptr;

		// the codeSize member is expected to be in bytes.
		shader_module_create_info.codeSize = sizeof(uint32_t) * file_contents.size();
		shader_module_create_info.pCode = file_contents.data();

		VkResult res = (vkCreateShaderModule(m_device, &shader_module_create_info, nullptr, &shader_module));
		if (res != VK_SUCCESS)
		{
			std::cout << "ERROR IN : " << file_path;
		}
	}

	void Engine::load_meshes()
	{
		// setup mesh for the triangle
		m_triangle_mesh.m_vertices.resize(3);

		m_triangle_mesh.m_vertices[0].m_position = {0.0f, -0.5f, 0.0f};
		m_triangle_mesh.m_vertices[1].m_position = {0.5f, 0.5f, 0.0f};
		m_triangle_mesh.m_vertices[2].m_position = {-0.5f, 0.5f, 0.0f};

		m_triangle_mesh.m_vertices[0].m_color = {0.0f, 1.0f, 0.0f};
		m_triangle_mesh.m_vertices[1].m_color = {0.0f, 0.0f, 1.0f};
		m_triangle_mesh.m_vertices[2].m_color = {1.0f, 0.0f, 0.0f};

		m_monkey_mesh.load_obj_file("../assets/monkey_smooth.obj");

		upload_meshes(m_monkey_mesh);
		upload_meshes(m_triangle_mesh);

		m_meshes["monkey_mesh"] = m_monkey_mesh;
		m_meshes["triangle_mesh"] = m_triangle_mesh;
	}

	// send data over to GPU accessible region
	void Engine::upload_meshes(Mesh& mesh)
	{
		// Allocate and create vertex buffer
		VkBufferCreateInfo buffer_create_info = {};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.pNext = nullptr;
		buffer_create_info.size = sizeof(Vertex) * mesh.m_vertices.size();
		buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		// Vma will allocate buffer such that it is writable by the CPU, and can be read from GPU
		VmaAllocationCreateInfo allocation_create_info = {};
		allocation_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		VK_CHECK(vmaCreateBuffer(m_vma_allocator, &buffer_create_info, &allocation_create_info, &mesh.m_allocated_buffer.m_buffer, &mesh.m_allocated_buffer.m_allocation_data, nullptr));
		m_deletors.push_function([=](){vmaDestroyBuffer(m_vma_allocator, mesh.m_allocated_buffer.m_buffer, mesh.m_allocated_buffer.m_allocation_data);});
		
		// since buffer is now created, the actual vertices data can be copied into a GPU readable memory region
		void *data;
		vmaMapMemory(m_vma_allocator, mesh.m_allocated_buffer.m_allocation_data, &data);

		memcpy(data, mesh.m_vertices.data(), mesh.m_vertices.size() * sizeof(Vertex));

		vmaUnmapMemory(m_vma_allocator, mesh.m_allocated_buffer.m_allocation_data);
	}

	void Engine::initialize_scene()
	{
		GameObject monkey;
		monkey.m_material = get_material("default_material");
		monkey.m_mesh = get_mesh("monkey_mesh");

		monkey.m_mesh_transform = glm::mat4(1.0f);

		m_game_objects.push_back(monkey);

		GameObject triangle;
		triangle.m_material = get_material("default_material");
		triangle.m_mesh = get_mesh("triangle_mesh");

		triangle.m_mesh_transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f)) * glm::mat4(1.0f);

		m_game_objects.push_back(triangle);
	}

	void Engine::create_material(const std::string& material_name, VkPipeline pipeline, VkPipelineLayout pipeline_layout)
	{
		Material new_material;
		new_material.m_pipeline = pipeline;
		new_material.m_pipeline_layout = pipeline_layout;

		m_materials[material_name] = new_material;
	}

	Material* Engine::get_material(const std::string& material_name)
	{
		auto it = m_materials.find(material_name);
		if (it == m_materials.end())
		{
			return nullptr;
		}

		return &(*it).second;
	}

	Mesh* Engine::get_mesh(const std::string& mesh_name)
	{
		auto it = m_meshes.find(mesh_name);
		if (it == m_meshes.end())
		{
			return nullptr;
		}

		return &(*it).second;
	}

	void Engine::draw_objects(VkCommandBuffer command_buffer, GameObject* game_object, int game_object_count)
	{
		// test out push constants
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -6.0f));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)m_config.m_window_width / m_config.m_window_height, 0.1f, 100.0f);

		// in vulkan the +y is facing down. thats why projection[1][1] is -1.
		projection[1][1] *= -1;

		Mesh* last_mesh = nullptr;
		Material* last_material = nullptr;

		for (int i = 0; i < game_object_count; i++)
		{
			GameObject& current_object = m_game_objects[i];

			if (current_object.m_material != last_material)
			{
				vkCmdBindPipeline(m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, current_object.m_material->m_pipeline);
				last_material = current_object.m_material;
			}

			glm::mat4 model_mat = current_object.m_mesh_transform;
			model_mat = glm::rotate(model_mat, glm::radians((float)m_frame_number), glm::vec3(1, 0, 0));

			PushConstants push_constants;
			push_constants.m_transform_mat = projection * view * model_mat;

			// push constants are inlayed onto a command buffer.
			vkCmdPushConstants(m_command_buffer, m_default_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &push_constants);

			if (current_object.m_mesh != last_mesh)
			{
				// bind vertex buffer with offset 0
				VkDeviceSize offset = 0;
				vkCmdBindVertexBuffers(m_command_buffer, 0, 1, &current_object.m_mesh->m_allocated_buffer.m_buffer, &offset);
				last_mesh = current_object.m_mesh;

			}

			vkCmdDraw(command_buffer, static_cast<uint32_t>(current_object.m_mesh->m_vertices.size()), 1, 0, 0);
		}
	}

	void Engine::clean()
	{
		vkDeviceWaitIdle(m_device);

		// vulkan objects must be destroyed in reverse the order they are created in.
		m_deletors.clear_deletor_list();

		vmaDestroyAllocator(m_vma_allocator);

		vkDestroyDevice(m_device, nullptr);
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger, nullptr);
		vkDestroyInstance(m_instance, nullptr);

		SDL_DestroyWindow(m_window);
		m_window = nullptr;

		SDL_Quit();
	}
}