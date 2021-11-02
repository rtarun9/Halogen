#pragma once

#include "vk_types.h"

#include <vector>

struct SDL_Window;
struct halo::DeletionList;

namespace halo
{
	class Engine
	{
	public:
		Engine();

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

	private:
		bool m_is_initialized{false};
		int m_frame_number{0};

		int m_window_width{1080};
		int m_window_height{720};

		SDL_Window *m_window{nullptr};
		VkExtent2D m_window_extent{static_cast<uint32_t>(m_window_width), static_cast<uint32_t>(m_window_height)};
		
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

		// utility / helper members
		DeletionList m_deletors;
	};
}