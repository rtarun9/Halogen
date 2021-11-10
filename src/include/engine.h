#pragma once

#include "types.h"
#include "mesh.h"

#include <vk_mem_alloc.h>

struct SDL_Window;

namespace halo
{
	struct Config
	{
		float m_window_width;
		float m_window_height;
		std::string m_window_name;
	};

	class Engine
	{
	public:
		Engine(const Config& config);
		~Engine();

		void initialize();
		void run();
		void clean();

	private:
		void render();
		
		void initialize_platform_backend();

		void initialize_vulkan();

		void initialize_swapchain();
		void initialize_depth_buffer();

		void initialize_command_objects();

		void initialize_renderpass();
		void initialize_framebuffers();

		void initialize_synchronization_objects();
		
		void initialize_pipeline();

		void load_shaders(const char *file_path, vk::ShaderModule& shader_module);
		void load_meshes();

		void upload_meshes(Mesh& mesh);

		void initialize_scene();

		void create_material(const std::string& material_name, vk::Pipeline pipeline, vk::PipelineLayout pipeline_layout);

		[[nodiscard]]
		Material* get_material(const std::string& material_name);

		[[nodiscard]]
		Mesh* get_mesh(const std::string& mesh_name);

		void draw_objects(VkCommandBuffer command_buffer, GameObject* game_object, int game_object_count);

	private:
		bool m_is_initialized{false};
		int m_frame_number{0};

		Config m_config;

		SDL_Window *m_window{nullptr};
		vk::Extent2D m_window_extent;
		
		// Main (core) vulkan handles
		vk::Instance m_instance;
		vk::DebugUtilsMessengerEXT m_debug_messenger;
		
		vk::PhysicalDevice m_physical_device;
		vk::Device m_device;

		vk::SurfaceKHR m_surface;

		// Swapchain related handles
		vk::SwapchainKHR m_swapchain;
		vk::Format m_swapchain_image_format;
		std::vector<vk::Image> m_swapchain_images;
		std::vector<vk::ImageView> m_swapchain_image_views;

		// related to depth buffer
		vk::Format m_depth_buffer_format;
		vk::Image m_depth_buffer_image;
		vk::ImageView m_depth_buffer_image_view;
		AllocatedImage m_depth_buffer_allocation;

		// Command objects
		vk::CommandPool m_main_command_pool;
		vk::CommandBuffer m_command_buffer;

		// Queue's and index into queue (both presentation + graphics)
		vk::Queue m_graphics_queue;
		uint32_t m_graphics_queue_index;

		// Frambuffer, render pass and (todo : depth buffer)
		vk::RenderPass m_render_pass;
		std::vector<vk::Framebuffer> m_framebuffers;

		// sync objects
		vk::Fence m_render_fence;

		vk::Semaphore m_render_semaphore;
		vk::Semaphore m_presentation_semaphore;

		// for rendering
		vk::Pipeline m_hardcoded_triangle_pipeline;
		vk::PipelineLayout m_hardcoded_triangle_layout;

		vk::Pipeline m_default_mesh_pipeline;
		vk::PipelineLayout m_default_mesh_layout;

		Mesh m_triangle_mesh;
		Mesh m_monkey_mesh;

		// VMA allocator
		VmaAllocator m_vma_allocator;

		// for handling cleanup convineintly
		DeletionList m_deletion_list;
	};
}