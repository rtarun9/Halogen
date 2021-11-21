#pragma once

#include "types.h"
#include "mesh.h"
#include "camera.h"

#include <vk_mem_alloc.h>

#include <unordered_map>
#include <iostream>

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

	// base engine class. All things are brought together here
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
		
		void init_platform_backend();

		void init_vulkan();

		void init_swapchain();
		void init_depth_buffer();

		void init_command_objects();

		void init_renderpass();
		void init_framebuffers();

		void init_synchronization_objects();

		void init_descriptors();
		
		void init_pipeline();

		void load_shaders(const char *file_path, vk::ShaderModule& shader_module);
		void load_meshes();

		void upload_meshes(Mesh& mesh);

		void init_scene();

		void create_material(const std::string& material_name, vk::Pipeline pipeline, vk::PipelineLayout pipeline_layout);

		[[nodiscard]]
		Material* get_material(const std::string& material_name);

		[[nodiscard]]
		Mesh* get_mesh(const std::string& mesh_name);

		void draw_objects(vk::CommandBuffer command_buffer, GameObject* game_object);

		// Util function to get the current frame (from the m_frame_data array) that is being used
		FrameData& get_current_frame_data();

		// Utils for buffer creation
		[[nodiscard]]
		AllocatedBuffer create_buffer(size_t allocation_size, vk::BufferUsageFlagBits usage, VmaMemoryUsage memory_usage);

		// Util function to pad size of alignment boundary, so that it the required alignment is based on device offset alignment
		[[nodiscard]]
		size_t pad_uniform_buffer(size_t actual_size);

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

		EnvironmentData m_environment_data;
		AllocatedBuffer m_environment_parameter_buffer;

		// descriptor related handles
		vk::DescriptorPool m_descriptor_pool;
		
		vk::DescriptorSetLayout m_global_descriptor_set_layout;
		vk::DescriptorSetLayout m_object_descriptor_set_layout;

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

		Camera m_camera;
		Timer m_timer;
	};
}