#pragma once

#include "types.h"
#include "mesh.h"

#include <vk_mem_alloc.h>

#include <vector>
#include <string>
#include <unordered_map>

struct SDL_Window;
struct halo::DeletionList;
struct halo::AllocatedBuffer;

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

		void initialize();
		void run();
		void clean();

	private:
		void render();

		void initialize_vulkan();

		void initialize_swapchain();
		void initialize_command_objects();

		void initialize_renderpass();
		void initialize_framebuffers();

		void initialize_synchronization_objects();
		
		void initialize_pipeline();

		void load_shaders(const char *file_path, VkShaderModule& shader_module);
		void load_meshes();

		void upload_meshes(Mesh& mesh);

		void initialize_scene();

		void create_material(const std::string& material_name, VkPipeline pipeline, VkPipelineLayout pipeline_layout);

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
		VkExtent2D m_window_extent;
		
		// Main vulkan handles
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debug_messenger;
		
		VkPhysicalDevice m_physical_device;
		VkDevice m_device;

		VkSurfaceKHR m_surface;

		VkSwapchainKHR  m_swapchain;
		VkFormat m_swapchain_image_format;
		std::vector<VkImage> m_swapchain_images;
		std::vector<VkImageView> m_swapchain_image_views;

		VkQueue m_graphics_queue;

		// index of the queue family that is required (this will be for both graphics and presentation).
		uint32_t m_graphics_queue_family;

		VkCommandPool m_command_pool;
		VkCommandBuffer m_command_buffer;

		VkRenderPass m_renderpass;
		std::vector<VkFramebuffer> m_framebuffers;

		// fence : GPU to CPU, semaphore : GPU to GPU
		VkFence m_render_fence;
		VkSemaphore m_render_semaphore;
		VkSemaphore m_present_semaphore;

		VkPipeline m_triangle_pipeline;
		VkPipeline m_inverted_triangle_pipeline;

		VkPipelineLayout m_triangle_pipeline_layout;

		// depth buffer variables
		AllocatedImage m_depth_buffer;
		VkImageView m_depth_image_view;
		VkFormat m_depth_format;

		// utility / helper members
		DeletionList m_deletors;

		// objects for the main scene
		VmaAllocator m_vma_allocator;

		VkPipeline m_default_pipeline;
		VkPipelineLayout m_default_pipeline_layout;

		Mesh m_triangle_mesh;
		Mesh m_monkey_mesh;

		// for the game objects
		std::vector<GameObject> m_game_objects;

		std::unordered_map<std::string, Material> m_materials;
		std::unordered_map<std::string, Mesh> m_meshes;
	};
}