#include "../include/engine.h"

#include "../include/vk_initializers.h"
#include "../include/vk_pipeline.h"

#include <VkBootstrap.h>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <fstream>

#define ONE_SECOND 1000000000
#define VK_CHECK(x) {VkResult res = x; if (res) {std::cout << "Vulkan Error : " << res << '\n'; __debugbreak(); exit(EXIT_FAILURE);}}

namespace halo
{
	Engine::Engine()
	{
	}

	void Engine::initialize()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			std::cout << "Failed to initialize SDL.";
			exit(EXIT_FAILURE);
		}

		SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WindowFlags::SDL_WINDOW_VULKAN | SDL_WindowFlags::SDL_WINDOW_SHOWN);

		m_window = SDL_CreateWindow("Halogen", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_window_extent.width, m_window_extent.height, window_flags);
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
		
		// start renderpass
		VkRenderPassBeginInfo renderpass_begin_info = {};
		renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderpass_begin_info.pNext = nullptr;
		renderpass_begin_info.clearValueCount = 1;
		renderpass_begin_info.pClearValues = &clear_value;
		renderpass_begin_info.framebuffer = m_framebuffers[swapchain_image_index];
		renderpass_begin_info.renderArea.extent = m_window_extent;
		renderpass_begin_info.renderArea.offset = {0, 0};
		renderpass_begin_info.renderPass = m_renderpass;

		vkCmdBeginRenderPass(m_command_buffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_triangle_pipeline);
		vkCmdDraw(m_command_buffer, 3, 1, 0, 0);

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
	}

	void Engine::initialize_command_objects()
	{
		VkCommandPoolCreateInfo command_pool_create_info = init::create_command_pool(m_graphics_queue_family);
		VK_CHECK(vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_command_pool));

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

		VkSubpassDescription subpass_desc = {};
		subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_desc.colorAttachmentCount = 1;
		subpass_desc.pColorAttachments = &color_attachment_ref;
		
		VkRenderPassCreateInfo renderpass_create_info = {};
		renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpass_create_info.pNext = nullptr;
		renderpass_create_info.attachmentCount = 1;
		renderpass_create_info.pAttachments = &color_attachment_desc;
		renderpass_create_info.subpassCount = 1;
		renderpass_create_info.pSubpasses = &subpass_desc;
		
		VK_CHECK(vkCreateRenderPass(m_device, &renderpass_create_info, nullptr, &m_renderpass));
	}
	
	void Engine::initialize_framebuffers()
	{
		// since most framebuffers will share a bunch of command stuff, create a common framebuffer create info with different attachments.
		VkFramebufferCreateInfo framebuffer_create_info = {};
		framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_create_info.pNext = nullptr;

		framebuffer_create_info.attachmentCount = 1;

		framebuffer_create_info.width = m_window_extent.width;
		framebuffer_create_info.height = m_window_extent.height;
		framebuffer_create_info.layers = 1;

		framebuffer_create_info.renderPass = m_renderpass;

		m_framebuffers.resize(m_swapchain_image_views.size());

		for (int i = 0; i < m_framebuffers.size(); i++)
		{
			framebuffer_create_info.pAttachments = &m_swapchain_image_views[i];
			VK_CHECK(vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_framebuffers[i]));
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

		VkSemaphoreCreateInfo semaphore_create_info = {};
		semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphore_create_info.pNext = nullptr;
		semaphore_create_info.flags = 0;

		VK_CHECK(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_render_semaphore));
		VK_CHECK(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_present_semaphore));
	}

	void Engine::initialize_pipeline()
	{
		VkShaderModule simple_triangle_vertex_module;
		load_shaders("../shaders/simple_triangle.vert.spv", simple_triangle_vertex_module);

		VkShaderModule simple_triangle_fragment_module;
		load_shaders("../shaders/simple_triangle.frag.spv", simple_triangle_fragment_module);
		


		VkPipelineLayoutCreateInfo pipeline_layout_info = init::create_pipeline_layout();
		VK_CHECK(vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_triangle_pipeline_layout));

		PipelineBuilder pipeline_builder;
		pipeline_builder.m_shader_stages.push_back(init::create_shader_stage(VK_SHADER_STAGE_FRAGMENT_BIT, simple_triangle_fragment_module));
		pipeline_builder.m_shader_stages.push_back(init::create_shader_stage(VK_SHADER_STAGE_VERTEX_BIT, simple_triangle_vertex_module));

		pipeline_builder.m_vertex_input_info = init::create_vertex_input_state();

		pipeline_builder.m_input_assembler = init::create_input_assembler();

		pipeline_builder.m_viewport.x = 0.0f;
		pipeline_builder.m_viewport.y = 0.0f;
		pipeline_builder.m_viewport.width = static_cast<float>(m_window_extent.width);
		pipeline_builder.m_viewport.height = static_cast<float>(m_window_extent.height);
		pipeline_builder.m_viewport.maxDepth = 1.0f;
		pipeline_builder.m_viewport.minDepth = 1.0f;

		pipeline_builder.m_scissor.offset = {0, 0};
		pipeline_builder.m_scissor.extent = m_window_extent;

		pipeline_builder.m_rasterizer_state_info = init::create_rasterizer_state();

		pipeline_builder.m_multisample_state_info = init::create_multisampling_info();

		pipeline_builder.m_color_blend_state_attachment = init::create_color_blend_state();

		pipeline_builder.m_pipeline_layout = m_triangle_pipeline_layout;

		m_triangle_pipeline = pipeline_builder.create_pipeline(m_device, m_renderpass);
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

	void Engine::clean()
	{
		vkDeviceWaitIdle(m_device);

		// vulkan objects must be destroyed in reverse the order they are created in.

		vkDestroySemaphore(m_device, m_render_semaphore, nullptr);
		vkDestroySemaphore(m_device, m_present_semaphore, nullptr);
		vkDestroyFence(m_device, m_render_fence, nullptr);

		// command pool will automatically destroy the command buffers allocated from it.
		vkDestroyCommandPool(m_device, m_command_pool, nullptr);

		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
		
		vkDestroyRenderPass(m_device, m_renderpass, nullptr);

		// images are destroed when swapchain is destroyed (since the swapchain own's those images).
		// framebuffers need to be deleted along with image views, since framebuffer holds attachments to them.
		for (int i = 0; i < m_swapchain_image_views.size(); i++)
		{
			vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
			vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);
		}

		vkDestroyDevice(m_device, nullptr);
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger, nullptr);
		vkDestroyInstance(m_instance, nullptr);

		SDL_DestroyWindow(m_window);
		m_window = nullptr;

		SDL_Quit();
	}
}