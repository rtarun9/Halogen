#pragma once

#include "types.h"
#include "mesh.h"

#include <vk_mem_alloc.h>

#include <unordered_map>

struct SDL_Window;

// frames in flight : decides single / double / triple buffering
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

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

		void draw_objects(vk::CommandBuffer command_buffer, GameObject* game_object);

		FrameData& get_current_frame_data();

	private:
		bool m_is_initialized{false};
		int m_frame_number{0};

		Config m_config;

		SDL_Window *m_window{nullptr};
		vk::Extent2D m_window_extent;
		
		// Main (core) vulkan handles
		vk::Instance m_instance;

		[[maybe_unused]]
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
		vk::Format m_depth_image_format;
		vk::Image m_depth_image;
		vk::ImageView m_depth_image_view;
		AllocatedImage m_depth_image_allocation;

		// Queue's and index into queue (both presentation + graphics)
		vk::Queue m_graphics_queue;
		uint32_t m_graphics_queue_index;

		// Frambuffer, render pass and (todo : depth buffer)
		vk::RenderPass m_render_pass;
		std::vector<vk::Framebuffer> m_framebuffers;

		FrameData m_frames[MAX_FRAMES_IN_FLIGHT];

		// for rendering
		vk::Pipeline m_triangle_pipeline;
		vk::PipelineLayout m_triangle_pipeline_layout;

		vk::Pipeline m_default_mesh_pipeline;
		vk::PipelineLayout m_default_mesh_layout;

		Mesh m_triangle_mesh;
		Mesh m_monkey_mesh;
		
		// scene management objects
		std::vector<GameObject> m_game_objects;
		std::unordered_map<std::string, Material> m_materials;
		std::unordered_map<std::string, Mesh> m_meshes;

		// VMA allocator
		VmaAllocator m_vma_allocator;

		// for handling cleanup convineintly
		DeletionList m_deletion_list;
	};
}