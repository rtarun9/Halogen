#pragma once

#include <glm/glm.hpp>

#include <deque>
#include <functional>

namespace halogen
{
	//Holds all queue families and queue extensions.
	struct  QueueFamilyIndices
	{
		uint32_t get_graphics_queue_family()
		{
			return m_graphics_queue_family.value();
		}

		uint32_t get_transfer_queue_family()
		{
			return m_transfer_queue_family.value();
		}

		uint32_t get_presentation_queue_family_extension()
		{
			return m_presentation_queue_family_extension.value();
		}

		bool is_queue_family_complete()
		{
			return m_graphics_queue_family.has_value() && m_transfer_queue_family.has_value();
		}

		std::optional<uint32_t> m_transfer_queue_family;
		std::optional<uint32_t> m_graphics_queue_family;

		std::optional<uint32_t> m_presentation_queue_family_extension;
	};

	//Basic struct for holding allocated buffer and its data
	struct AllocatedBuffer
	{
		//Vulkan GPU side buffer.
		VkBuffer m_buffer;

		//If vma wasn't used, this would be there
		//VkDeviceMemory m_vertex_buffer_memory;

		//Hold state VMA uses (size, memory buffer was allocated from).
		VmaAllocation m_allocation;
	};

	//Basic struct for implementing depth image
	struct AllocatedImage
	{
		VkImage m_image;
		VmaAllocation m_allocation;
	};

	//Destruction queue : easy automated way of calling vkDestroyXXX commands in the right order.
    struct DeletionQueue
    {
    public:
        std::deque<std::function<void()>> deletors;

        void push_function(std::function<void()>&& function)
        {
            deletors.push_back(function);
        }

        //This will call the lambda functions in the same order that they were queued in.
        //Not a very efficient way, but gets the job done.
        void flush()
        {
            //Using rbegin and rend here since we want to iterate over the double ended queue in reverse, because
            //Usually objects in vulkan are deleted in the reverse order they were implemented.
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
            {
                (*it)();
            }

            deletors.clear();
        }
    };

	//Basic wrappers for Vulkan objects
	namespace wrapper
	{
		struct CameraData
		{
			glm::mat4 m_view_mat;
			glm::mat4 m_projection_mat;

			//To avoid multiplication of matrix in shader
			glm::mat4 m_vp_mat;
		};

		//Size : 16 * 3 = 48 (does not meet any of the alignments for GPU).
		struct EnvironmentData
		{
			glm::vec4 m_fog_color;
			glm::vec4 m_fog_distance;
			glm::vec4 m_ambient_color;
		};

		struct FrameData
		{
			VkCommandPool m_command_pool;
			VkCommandBuffer m_command_buffer;

			VkSemaphore m_present_semaphore;
			VkSemaphore m_render_semaphore;
			VkFence m_render_fence;

			//Buffer for the camera
			AllocatedBuffer m_camera_buffer;
			VkDescriptorSet m_global_descriptor;

			//Holds where the location of mapped memory
			void *m_mapped_camera_buffer;
			void *m_mapped_environment_buffer;
		};

		struct Swapchain
		{
			VkSwapchainKHR m_swapchain{};
			VkFormat m_swapchain_image_format{};
			std::vector<VkImage> m_swapchain_images{};
			std::vector<VkImageView> m_swapchain_image_views{};
		};

		struct DepthImage
		{
			VkImageView m_depth_image_view{};
			VkFormat m_depth_format{};
			AllocatedImage m_depth_image{};
		};
	}
}