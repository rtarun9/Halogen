#include "../include/engine.h"

#include "../include/initializers.h"

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <glm/gtc/matrix_transform.hpp>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <fstream>

#define ONE_SECOND 1000000000

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
		
	}

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

	void Engine::initialize_command_objects()
	{
		vk::CommandPoolCreateInfo command_pool_create_info = init::create_command_pool(m_graphics_queue_index);
		m_main_command_pool = m_device.createCommandPool(command_pool_create_info);

		vk::CommandBufferAllocateInfo command_buffer_allocate_info = init::create_command_buffer_allocate(m_main_command_pool);
	 	m_command_buffer = m_device.allocateCommandBuffers(command_buffer_allocate_info)[0];
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

		// Description for only subpass of renderpass
		vk::SubpassDescription subpass_desc = {};
		subpass_desc.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass_desc.colorAttachmentCount = 1;
		subpass_desc.pColorAttachments = &color_attachment_ref;

		vk::RenderPassCreateInfo render_pass_create_info = {};
		render_pass_create_info.attachmentCount = 1;
		render_pass_create_info.pAttachments = &color_attachment_desc;
		render_pass_create_info.subpassCount = 1;
		render_pass_create_info.pSubpasses = &subpass_desc;
		
		m_render_pass = m_device.createRenderPass(render_pass_create_info);
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
			framebuffer_create_info.attachmentCount = 1;
			framebuffer_create_info.pAttachments = &m_swapchain_image_views[i];

			m_framebuffers[i] = m_device.createFramebuffer(framebuffer_create_info);
		}
	}

	void Engine::initialize_synchronization_objects()
	{
		
	}

	void Engine::initialize_pipeline()
	{
		
	}

	void Engine::load_shaders(const char* file_path, VkShaderModule& shader_module)
	{
		
	}

	void Engine::load_meshes()
	{
		
	}

	void Engine::initialize_scene()
	{
		
	}

	void Engine::create_material(const std::string& material_name, VkPipeline pipeline, VkPipelineLayout pipeline_layout)
	{
		
	}

	Material* Engine::get_material(const std::string& material_name)
	{
		return nullptr;
	}

	Mesh* Engine::get_mesh(const std::string& mesh_name)
	{
		return nullptr;
	}

	void Engine::draw_objects(VkCommandBuffer command_buffer, GameObject* game_object, int game_object_count)
	{
		
	}

	void Engine::clean()
	{
		m_device.waitIdle();

		if (m_is_initialized)
		{
			m_device.destroySwapchainKHR(m_swapchain);
			
			m_device.destroyRenderPass(m_render_pass);

			m_device.destroyCommandPool(m_main_command_pool);

			for (size_t i = 0; i < m_swapchain_image_views.size(); i++)
			{
				m_device.destroyFramebuffer(m_framebuffers[i]);
				m_device.destroyImageView(m_swapchain_image_views[i]);
			}
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