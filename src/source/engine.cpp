#include "../include/engine.h"

#include "../include/initializers.h"
#include "../include/pipeline.h"

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <glm/gtc/matrix_transform.hpp>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <format>
#include <iostream>
#include <algorithm>
#include <fstream>

#define ONE_SECOND 1000000000

#define VK_CHECK(x)                                                    \
do																	   \
{																	   \
	vk::Result res = x;												   \
	if (res != vk::Result::eSuccess)								   \
	{																   \
		std::cerr << "Vulkan Error : " << res << std::endl;			   \
	}																   \
}																	   \
while(0)															   \

// macro for making push back's to deletion queue a bit more readable
#define CREATE_LAMBDA_FUNCTION(function) ([=](){function;})

namespace halo
{
	Engine::Engine(const Config& config): m_config(config)
	{
		m_window_extent.setWidth(static_cast<uint32_t>(m_config.m_window_width));
		m_window_extent.setHeight(static_cast<uint32_t>(m_config.m_window_height));

		initialize();
		run();
	}

	Engine::~Engine()
	{
		clean();
	}

	void Engine::initialize()
	{
		initialize_platform_backend();
		
		initialize_vulkan();

		initialize_swapchain();
		initialize_depth_buffer();

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
		// wait until GPU has rendered the last frame
		VK_CHECK(m_device.waitForFences(get_current_frame_data().m_render_fence, true, ONE_SECOND));
		m_device.resetFences(get_current_frame_data().m_render_fence);

		// presentation semaphore will be signalled when swapchain image is acquired.
		vk::ResultValue<uint32_t> swapchain_image_index = m_device.acquireNextImageKHR(m_swapchain, ONE_SECOND, get_current_frame_data().m_presentation_semaphore, nullptr);

		// begin rendering commands
		get_current_frame_data().m_command_buffer.reset();
		
		vk::CommandBuffer command_buffer = get_current_frame_data().m_command_buffer;

		vk::CommandBufferBeginInfo command_buffer_begin_info = {};

		// flags set to one time submit since it is being re recorded every frame
		command_buffer_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
		
		command_buffer.begin(command_buffer_begin_info);
	
		vk::ClearColorValue clear_color;
		clear_color.setFloat32({0.0f, 0.0f, (float)abs(sin(SDL_GetTicks() / 120.0f))});

		vk::ClearDepthStencilValue depth_clear;
		depth_clear.setDepth(1.0f);

		vk::ClearValue clear_values[2] = {clear_color, depth_clear};

		// begin renderpass
		vk::RenderPassBeginInfo render_pass_begin_info = {};
		render_pass_begin_info.renderPass = m_render_pass;
		render_pass_begin_info.renderArea.extent = m_window_extent;
		render_pass_begin_info.renderArea.offset = vk::Offset2D{0, 0};
		render_pass_begin_info.clearValueCount = 2;
		render_pass_begin_info.pClearValues = clear_values;
		render_pass_begin_info.framebuffer = m_framebuffers[swapchain_image_index.value];

		command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
		
		draw_objects(command_buffer, m_game_objects.data());

		command_buffer.endRenderPass();

		command_buffer.end();

		// submit to GPU

		// note : wait on presentation semaphore, since it is signaled when swapchain is ready
		// note : signal render sempaphore, when rendering is finished.

		vk::SubmitInfo submit_info = {};
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &get_current_frame_data().m_presentation_semaphore;

		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &get_current_frame_data().m_render_semaphore;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vk::PipelineStageFlags dst_wait_stage_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		submit_info.pWaitDstStageMask = &dst_wait_stage_mask;

		// once all command buffers have completed thier execution, m_render_fence is signalled.
		m_graphics_queue.submit(submit_info, get_current_frame_data().m_render_fence);

		// wait on render semaphore before presentation
		// m_render_fence is not needed to be explicity set here since presentation waits on m_render_semaphore, and so does m_render_fence.
		vk::PresentInfoKHR present_info = {};
		present_info.pSwapchains = &m_swapchain;
		present_info.swapchainCount = 1;

		present_info.pWaitSemaphores = &get_current_frame_data().m_render_semaphore;
		present_info.waitSemaphoreCount = 1;

		present_info.pImageIndices = &swapchain_image_index.value;

		VK_CHECK(m_graphics_queue.presentKHR(present_info));

		m_frame_number++;
	};

	void Engine::initialize_platform_backend()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			std::cout << "Failed to initialize SDL.";
			exit(EXIT_FAILURE);
		}

		SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WindowFlags::SDL_WINDOW_VULKAN | SDL_WindowFlags::SDL_WINDOW_SHOWN);

		m_window = SDL_CreateWindow(m_config.m_window_name.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			m_window_extent.width, m_window_extent.height,
			window_flags);

		if (m_window == nullptr)
		{
			std::cout << "Failed to create window";
			exit(EXIT_FAILURE);
		}
	}

	// use vkbootstrap for initializing core Vk objects. Will be replaced later.
	void Engine::initialize_vulkan()
	{
		vkb::InstanceBuilder instance_builder;

		auto instance = instance_builder.set_app_name("Halogen")
			.request_validation_layers(true)
			.require_api_version(1, 1, 0)
			.use_default_debug_messenger()
			.build();

		vkb::Instance vkb_instance = instance.value();

		m_instance = vkb_instance.instance;
		m_debug_messenger = vkb_instance.debug_messenger;

		// temporary workaround since SDL_Vulkan is non-compatible with vulkan.hpp
		VkSurfaceKHR temp_surface = VK_NULL_HANDLE;
		SDL_Vulkan_CreateSurface(m_window, m_instance, &temp_surface);
		m_surface = temp_surface;

		// physical device is selected after surface since we want to be able to render to that surface.
		// by default vkbootstrap will apparently try to choose the dedicated GPU, which is preferable.
		vkb::PhysicalDeviceSelector physical_device_selector {vkb_instance};
		vkb::PhysicalDevice vkb_physical_device = physical_device_selector.set_minimum_version(1, 1)
			.set_surface(m_surface)
			.select()
			.value();

		vkb::DeviceBuilder device_builder {vkb_physical_device};

		vkb::Device vkb_device = device_builder.build().value();

		m_device = vkb_device.device;
		m_physical_device = vkb_physical_device.physical_device;

		// display which device was chosen
		vk::PhysicalDeviceProperties device_properties = m_physical_device.getProperties();
		
		std::cout << "Device chosen : "  << device_properties.deviceName<< '\n';
		
		// create the vma allocator
		VmaAllocatorCreateInfo vma_allocator_create_info = {};
		vma_allocator_create_info.device = m_device;
		vma_allocator_create_info.instance = m_instance;
		vma_allocator_create_info.physicalDevice = m_physical_device;
		vmaCreateAllocator(&vma_allocator_create_info, &m_vma_allocator);

		// acquire queue and its index
		m_graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
		m_graphics_queue_index = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
	}

	// uses vkbootstrap for swapchain initialization.
	void Engine::initialize_swapchain()
	{
		vkb::SwapchainBuilder swapchain_builder{m_physical_device, m_device, m_surface};

		vkb::Swapchain vkb_swapchain = swapchain_builder
			.use_default_format_selection()
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_RELAXED_KHR)
			.set_desired_extent(m_window_extent.width, m_window_extent.height)
			.build()
			.value();

		m_swapchain = vkb_swapchain.swapchain;

		m_swapchain_images = vkb_swapchain.get_swapchain_images();
		m_swapchain_image_views = vkb_swapchain.get_swapchain_image_views();
	
		m_swapchain_image_format = vk::Format(vkb_swapchain.image_format);
	}

	void Engine::initialize_depth_buffer()
	{
		m_depth_image_format = vk::Format::eD32Sfloat;
		
		vk::Extent3D extent = {};
		extent.setWidth(m_window_extent.width);
		extent.setHeight(m_window_extent.height);
		extent.setDepth(1.0f);

		vk::ImageCreateInfo depth_image_create_info = init::create_image_info(m_depth_image_format, extent, vk::ImageUsageFlagBits::eDepthStencilAttachment);

		VmaAllocationCreateInfo depth_image_allocation_create_info = {};
		depth_image_allocation_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		depth_image_allocation_create_info.requiredFlags = static_cast<VkMemoryPropertyFlags>(vk::MemoryPropertyFlagBits::eDeviceLocal);

		// since Vma does not natively support vulkan.hpp, some workarounds must be used.
		auto image_create_info = static_cast<VkImageCreateInfo>(depth_image_create_info);
		auto image = static_cast<VkImage>(m_depth_image);

		vmaCreateImage(m_vma_allocator, &image_create_info, &depth_image_allocation_create_info, &image, &m_depth_image_allocation.m_allocation_data, nullptr);

		m_depth_image = image;

		vk::ImageViewCreateInfo depth_image_view_create_info = init::create_image_view_info(m_depth_image_format, m_depth_image, vk::ImageAspectFlagBits::eDepth);
		m_depth_image_view = m_device.createImageView(depth_image_view_create_info);

		m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyImageView(m_depth_image_view)));
		m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(vmaDestroyImage(m_vma_allocator, image, nullptr)));

	}

	void Engine::initialize_command_objects()
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{

			vk::CommandPoolCreateInfo command_pool_create_info = init::create_command_pool(m_graphics_queue_index);
			m_frames[i].m_primary_command_pool = m_device.createCommandPool(command_pool_create_info);
			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyCommandPool(m_frames[i].m_primary_command_pool)));

			vk::CommandBufferAllocateInfo command_buffer_allocate_info = init::create_command_buffer_allocate(m_frames[i].m_primary_command_pool);
	 		m_frames[i].m_command_buffer = m_device.allocateCommandBuffers(command_buffer_allocate_info)[0];
		}
	}

	// renderpass stores the state of images rendering into, and the state needed to setup the target framebuffer for rendering.
	void Engine::initialize_renderpass()
	{
		// create the color attachment
		vk::AttachmentDescription color_attachment_desc = {};
		color_attachment_desc.format = m_swapchain_image_format;
		color_attachment_desc.samples = vk::SampleCountFlagBits::e1;
		color_attachment_desc.loadOp = vk::AttachmentLoadOp::eClear;
		color_attachment_desc.storeOp = vk::AttachmentStoreOp::eStore;
		color_attachment_desc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		color_attachment_desc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		color_attachment_desc.initialLayout = vk::ImageLayout::eUndefined;

		// image should be ready for presentation
		color_attachment_desc.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		// reference to color attachment
		vk::AttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;

		// create the depth attachment
		vk::AttachmentDescription depth_attachment_desc = {};
		depth_attachment_desc.format = m_depth_image_format;
		depth_attachment_desc.samples = vk::SampleCountFlagBits::e1;
		depth_attachment_desc.loadOp = vk::AttachmentLoadOp::eClear;
		depth_attachment_desc.storeOp = vk::AttachmentStoreOp::eStore;

		depth_attachment_desc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		depth_attachment_desc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

		depth_attachment_desc.initialLayout = vk::ImageLayout::eUndefined;
		depth_attachment_desc.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		// reference to depth attachment
		vk::AttachmentReference depth_attachment_ref = {};
		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		// Description for only subpass of renderpass
		vk::SubpassDescription subpass_desc = {};
		subpass_desc.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass_desc.colorAttachmentCount = 1;
		subpass_desc.pColorAttachments = &color_attachment_ref;

		subpass_desc.pDepthStencilAttachment = &depth_attachment_ref;

		vk::AttachmentDescription attachments[] = {color_attachment_desc, depth_attachment_desc};

		vk::RenderPassCreateInfo render_pass_create_info = {};
		render_pass_create_info.attachmentCount = 2;
		render_pass_create_info.pAttachments = &attachments[0];
		render_pass_create_info.subpassCount = 1;
		render_pass_create_info.pSubpasses = &subpass_desc;
		
		m_render_pass = m_device.createRenderPass(render_pass_create_info);
		m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyRenderPass(m_render_pass)));
	}
	
	// acts as a link between the attachments of the renderpassand the real images that they should render to.
	// contains color, depth, stencil and other buffers.
	void Engine::initialize_framebuffers()
	{
		// since most properties will be shared, all fb create info structs will be made by modifying this template
		vk::FramebufferCreateInfo framebuffer_create_info = {};
		framebuffer_create_info.renderPass = m_render_pass;
		framebuffer_create_info.width = m_window_extent.width;
		framebuffer_create_info.height = m_window_extent.height;
		framebuffer_create_info.layers = 1;

		const size_t swapchain_image_count = m_swapchain_images.size();
		m_framebuffers = std::vector<vk::Framebuffer>(swapchain_image_count);

		for (size_t i = 0; i < m_swapchain_image_views.size(); i++)
		{
			vk::ImageView attachments[] = {m_swapchain_image_views[i], m_depth_image_view};

			framebuffer_create_info.attachmentCount = 2;
			framebuffer_create_info.pAttachments = attachments;

			m_framebuffers[i] = m_device.createFramebuffer(framebuffer_create_info);
			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyFramebuffer(m_framebuffers[i])));
			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyImageView(m_swapchain_image_views[i])));
		}
	}

	// sync objects : fence (GPU to CPU), semaphore (GPU to GPU)
	void Engine::initialize_synchronization_objects()
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vk::FenceCreateInfo fence_create_info = init::create_fence();
			m_frames[i].m_render_fence = m_device.createFence(fence_create_info);
			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyFence(m_frames[i].m_render_fence)));

			vk::SemaphoreCreateInfo semaphore_create_info = init::create_semaphore();
			m_frames[i].m_render_semaphore = m_device.createSemaphore(semaphore_create_info);
			m_frames[i].m_presentation_semaphore = m_device.createSemaphore(semaphore_create_info);

			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroySemaphore(m_frames[i].m_render_semaphore)));
			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroySemaphore(m_frames[i].m_presentation_semaphore)));
		}
	}

	void Engine::initialize_pipeline()
	{
		// for triangle's
		{
			// create shader modules
			vk::ShaderModule default_vert_module;
			load_shaders("../shaders/default_mesh.vert.spv", default_vert_module);

			vk::ShaderModule triangle_test_frag;
			load_shaders("../shaders/triangle_test.frag.spv", triangle_test_frag);

			PipelineBuilder pipeline_builder = {};
			pipeline_builder.m_shader_stages.push_back(init::create_shader_stage(vk::ShaderStageFlagBits::eVertex, default_vert_module));
			pipeline_builder.m_shader_stages.push_back(init::create_shader_stage(vk::ShaderStageFlagBits::eFragment, triangle_test_frag));

			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyShaderModule(default_vert_module)));
			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyShaderModule(triangle_test_frag)));

			pipeline_builder.m_vertex_input_info = init::create_vertex_input_state();

			VertexInputLayoutDescription vertex_input_layout_description = Vertex::get_vertex_input_layout_description();

			pipeline_builder.m_vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_layout_description.m_bindings.size());
			pipeline_builder.m_vertex_input_info.pVertexBindingDescriptions = vertex_input_layout_description.m_bindings.data();

			pipeline_builder.m_vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_layout_description.m_attributes.size());
			pipeline_builder.m_vertex_input_info.pVertexAttributeDescriptions = vertex_input_layout_description.m_attributes.data();

			pipeline_builder.m_input_assembler = init::create_input_assembler();

			pipeline_builder.m_viewport.x = 0.0f;
			pipeline_builder.m_viewport.y = 0.0f;
			pipeline_builder.m_viewport.width = static_cast<float>(m_window_extent.width);
			pipeline_builder.m_viewport.height = static_cast<float>(m_window_extent.height);
			pipeline_builder.m_viewport.minDepth = 0.0f;
			pipeline_builder.m_viewport.maxDepth = 1.0f;

			pipeline_builder.m_scissor.offset = vk::Offset2D{0, 0};
			pipeline_builder.m_scissor.extent = m_window_extent;

			pipeline_builder.m_rasterizer_state_info = init::create_rasterizer_state();
			pipeline_builder.m_multisample_state_info = init::create_multisampling_info();
			pipeline_builder.m_color_blend_state_attachment = init::create_color_blend_state();
			pipeline_builder.m_depth_stencil_state_info = init::create_depth_stencil_state();
			
			// create push constant (configure so that it can hold a view projection matrix)
			vk::PushConstantRange push_constant_range = {};
			push_constant_range.size = sizeof(MeshPushConstants);
			push_constant_range.offset = 0;
			push_constant_range.stageFlags = vk::ShaderStageFlagBits::eVertex;

			//m_default_mesh_layout = m_device.createPipelineLayout(init::create_pipeline_layout());
			vk::PipelineLayoutCreateInfo pipeline_layout_create_info = {};
			pipeline_layout_create_info.pushConstantRangeCount = 1;
			pipeline_layout_create_info.pPushConstantRanges = &push_constant_range;

			m_triangle_pipeline_layout = m_device.createPipelineLayout(pipeline_layout_create_info);
			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyPipelineLayout(m_triangle_pipeline_layout)));

			pipeline_builder.m_pipeline_layout = m_triangle_pipeline_layout;

			m_triangle_pipeline = pipeline_builder.create_pipeline(m_device, m_render_pass);
			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyPipeline(m_triangle_pipeline)));

			create_material("triangle_material", m_triangle_pipeline, m_triangle_pipeline_layout);
		}

		// creation for mesh pipeline and layout (with vertex buffer and push constants)
		{
			// create shader modules
			vk::ShaderModule mesh_vert_module;
			load_shaders("../shaders/default_mesh.vert.spv", mesh_vert_module);

			vk::ShaderModule mesh_frag_module;
			load_shaders("../shaders/default_mesh.frag.spv", mesh_frag_module);

			PipelineBuilder pipeline_builder = {};
			pipeline_builder.m_shader_stages.push_back(init::create_shader_stage(vk::ShaderStageFlagBits::eVertex, mesh_vert_module));
			pipeline_builder.m_shader_stages.push_back(init::create_shader_stage(vk::ShaderStageFlagBits::eFragment, mesh_frag_module));

			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyShaderModule(mesh_vert_module)));
			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyShaderModule(mesh_frag_module)));

			pipeline_builder.m_vertex_input_info = init::create_vertex_input_state();

			VertexInputLayoutDescription vertex_input_layout_description = Vertex::get_vertex_input_layout_description();

			pipeline_builder.m_vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_layout_description.m_bindings.size());
			pipeline_builder.m_vertex_input_info.pVertexBindingDescriptions = vertex_input_layout_description.m_bindings.data();

			pipeline_builder.m_vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_layout_description.m_attributes.size());
			pipeline_builder.m_vertex_input_info.pVertexAttributeDescriptions = vertex_input_layout_description.m_attributes.data();

			pipeline_builder.m_input_assembler = init::create_input_assembler();

			pipeline_builder.m_viewport.x = 0.0f;
			pipeline_builder.m_viewport.y = 0.0f;
			pipeline_builder.m_viewport.width = static_cast<float>(m_window_extent.width);
			pipeline_builder.m_viewport.height = static_cast<float>(m_window_extent.height);
			pipeline_builder.m_viewport.minDepth = 0.0f;
			pipeline_builder.m_viewport.maxDepth = 1.0f;

			pipeline_builder.m_scissor.offset = vk::Offset2D{ 0, 0 };
			pipeline_builder.m_scissor.extent = m_window_extent;

			pipeline_builder.m_rasterizer_state_info = init::create_rasterizer_state();
			pipeline_builder.m_multisample_state_info = init::create_multisampling_info();
			pipeline_builder.m_color_blend_state_attachment = init::create_color_blend_state();
			pipeline_builder.m_depth_stencil_state_info = init::create_depth_stencil_state();

			// create push constant (configure so that it can hold a view projection matrix)
			vk::PushConstantRange push_constant_range = {};
			push_constant_range.size = sizeof(MeshPushConstants);
			push_constant_range.offset = 0;
			push_constant_range.stageFlags = vk::ShaderStageFlagBits::eVertex;

			//m_default_mesh_layout = m_device.createPipelineLayout(init::create_pipeline_layout());
			vk::PipelineLayoutCreateInfo pipeline_layout_create_info = {};
			pipeline_layout_create_info.pushConstantRangeCount = 1;
			pipeline_layout_create_info.pPushConstantRanges = &push_constant_range;

			m_default_mesh_layout = m_device.createPipelineLayout(pipeline_layout_create_info);
			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyPipelineLayout(m_default_mesh_layout)));

			pipeline_builder.m_pipeline_layout = m_default_mesh_layout;

			m_default_mesh_pipeline = pipeline_builder.create_pipeline(m_device, m_render_pass);
			m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(m_device.destroyPipeline(m_default_mesh_pipeline)));
			create_material("default_material", m_default_mesh_pipeline, m_default_mesh_layout);
		}
	}

	void Engine::load_shaders(const char* file_path, vk::ShaderModule& shader_module)
	{
		// std::ios::ate : file pointer at the end of file (easy to find file size) and std::ios::binary because of spirv.
		std::ifstream file(file_path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error(std::string("Failed to find file" + std::string(file_path)));
		}

		size_t file_size_bytes = static_cast<size_t>(file.tellg());

		// spriv expects buffer to be uint32's
		std::vector<uint32_t> buffer(file_size_bytes / sizeof(uint32_t));

		file.seekg(0);

		file.read((char*)buffer.data(), file_size_bytes);

		file.close();

		vk::ShaderModuleCreateInfo shader_module_create_info = {};
		shader_module_create_info.codeSize = file_size_bytes;
		shader_module_create_info.pCode = buffer.data();

		shader_module = m_device.createShaderModule(shader_module_create_info);
	}

	void Engine::load_meshes()
	{
		m_triangle_mesh.m_vertices.resize(3);
		m_triangle_mesh.m_vertices[0].m_position = {-0.5f, -0.5f, 0.0f};
		m_triangle_mesh.m_vertices[0].m_color = {1.0f, 0.0f, 0.0f};

		m_triangle_mesh.m_vertices[1].m_position = {0.5f, -0.5f, 0.0f};
		m_triangle_mesh.m_vertices[1].m_color = {0.0f, 1.0f, 0.0f};

		m_triangle_mesh.m_vertices[2].m_position = {0.0f, 0.5f, 0.0f};
		m_triangle_mesh.m_vertices[2].m_color = {0.0f, 0.0f, 1.0f};

		m_monkey_mesh.load_obj_from_file("../assets/monkey_flat.obj");

		upload_meshes(m_triangle_mesh);
		upload_meshes(m_monkey_mesh);

		m_meshes["monkey_mesh"] = m_monkey_mesh;
		m_meshes["triangle_mesh"] = m_triangle_mesh;
	}

	void Engine::upload_meshes(Mesh& mesh)
	{
		// allocate vertex buffer
		vk::BufferCreateInfo buffer_create_info = {};
		buffer_create_info.size = mesh.m_vertices.size() * sizeof(Vertex);
		buffer_create_info.usage = vk::BufferUsageFlagBits::eVertexBuffer;

		// use VMA to specify that this data is written nto by CPU and accessed by GPU
		VmaAllocationCreateInfo vma_allocation_create_info = {};
		vma_allocation_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		// workaround since vma does not natively support vulkan.hpp
		VkBufferCreateInfo vertex_buffer_create_info = static_cast<VkBufferCreateInfo>(buffer_create_info);
		VkBuffer vertex_buffer;

		vk::Result res = vk::Result(vmaCreateBuffer(m_vma_allocator, &vertex_buffer_create_info, &vma_allocation_create_info, &vertex_buffer, &mesh.m_allocated_buffer.m_allocation_data, nullptr));
		VK_CHECK(res);
		m_deletion_list.push_function(CREATE_LAMBDA_FUNCTION(vmaDestroyBuffer(m_vma_allocator, vertex_buffer, mesh.m_allocated_buffer.m_allocation_data)));
	
		mesh.m_allocated_buffer.m_buffer = vertex_buffer;

		// now that we have memory spot for vertex data, copy vertices into this GPU readable location
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
		triangle.m_material = get_material("triangle_material");
		triangle.m_mesh = get_mesh("triangle_mesh");
		triangle.m_mesh_transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -3.3f)) * glm::scale(glm::mat4(1.0f), glm::vec3(4.0f));

		m_game_objects.push_back(triangle);
	}

	void Engine::create_material(const std::string& material_name, vk::Pipeline pipeline, vk::PipelineLayout pipeline_layout)
	{
		Material material;
		material.m_pipeline = pipeline;
		material.m_pipeline_layout = pipeline_layout;

		m_materials[material_name] = material;
	}

	Material* Engine::get_material(const std::string& material_name)
	{
		auto it = m_materials.find(material_name);
		if (it != m_materials.end())
		{
			return &m_materials[material_name];
		}

		throw std::runtime_error("Failed to find material with name : " + material_name);

		return nullptr;
	}

	Mesh* Engine::get_mesh(const std::string& mesh_name)
	{
		auto it = m_meshes.find(mesh_name);
		if (it != m_meshes.end())
		{
			return &m_meshes[mesh_name];
		}

		throw std::runtime_error("Failed to find mesh with name : " + mesh_name);

		return nullptr;
	}

	void Engine::draw_objects(vk::CommandBuffer command_buffer, GameObject* game_object)
	{		
		glm::vec3 camera_position{0.0f, 0.0f, -10.0f};

		glm::mat4 view = glm::translate(glm::mat4{1.0f}, camera_position);
		
		glm::mat4 projection_mat = glm::perspective(glm::radians(45.0f), static_cast<float>(m_window_extent.width) / m_window_extent.height, 0.1f, 100.0f);
		projection_mat[1][1] *= -1;
		
		Mesh *last_mesh = nullptr;
		Material *last_material = nullptr;

		for (const auto& game_object : m_game_objects)
		{
			if (game_object.m_material != last_material)
			{
				command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, game_object.m_material->m_pipeline);
				last_material = game_object.m_material;
			}

			glm::mat4 model_mat = glm::rotate(game_object.m_mesh_transform, glm::radians(float(m_frame_number)), glm::vec3(0.0f ,1.0f, 0.0f));
			
			MeshPushConstants push_constants;
			push_constants.m_transform_mat = projection_mat * view * model_mat;
			command_buffer.pushConstants(last_material->m_pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(MeshPushConstants), &push_constants);

			if (game_object.m_mesh != last_mesh)
			{
				vk::DeviceSize offset{0};
				command_buffer.bindVertexBuffers(0, game_object.m_mesh->m_allocated_buffer.m_buffer, offset);

				last_mesh = game_object.m_mesh;
			}
			
			command_buffer.draw(game_object.m_mesh->m_vertices.size(), 1, 0, 0);
		}
	}

	FrameData& Engine::get_current_frame_data()
	{
		return m_frames[m_frame_number % MAX_FRAMES_IN_FLIGHT];
	}
	void Engine::clean()
	{
		m_device.waitIdle();

		if (m_is_initialized)
		{
			m_device.destroySwapchainKHR(m_swapchain);

			// clear deletion list
			m_deletion_list.clear_deletor_list();
		}

		m_device.destroy();
		
		m_instance.destroySurfaceKHR(m_surface);

		vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger, nullptr);
		m_instance.destroy();

		SDL_DestroyWindow(m_window);
		m_window = nullptr;

		SDL_Quit();
	}
}