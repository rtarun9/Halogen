#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include <glm/glm.hpp>

#include <vector>
#include <functional>

namespace halo
{
	struct Mesh;
	struct Material;

	struct DeletionList
	{
		std::vector<std::function<void()>> deletors;

		// significance of && here : lvalue is copied, while r value is moved.
		// mostly (almost always) the capture will be by value. Reason for this is that the objects will go outofscope in that particular function block, and we want to preserve it.
		// hence we do a copy by value.
		void push_function(std::function<void()>&& function)
		{
			deletors.push_back(function);
		}

		void clear_deletor_list()
		{
			for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
			{
				(*it)();
			}

			deletors.clear();
		}
	};

	struct MeshPushConstants
	{
		// transform_mat : projection_matrix * view_matrix * model_mat
		glm::mat4 m_transform_mat;
	};

	// struct for the buffer allocated + some information like where it has been allocated, heap type, and VMA state.
	struct AllocatedBuffer
	{
		vk::Buffer m_buffer;
		VmaAllocation m_allocation_data;
	};

	// struct for image allocated
	struct AllocatedImage
	{
		vk::Image m_image;
		VmaAllocation m_allocation_data;
	};

	// struct for game objects :  

	// Material : pipeline and pipeline layout 
	struct Material
	{
		vk::Pipeline m_pipeline;
		vk::PipelineLayout m_pipeline_layout;
	};

	struct GameObject
	{
		Mesh *m_mesh;
		Material *m_material;
		glm::mat4 m_mesh_transform;
	};

	struct FrameData
	{
		vk::Semaphore m_presentation_semaphore;
		vk::Semaphore m_render_semaphore;

		vk::Fence m_render_fence;

		vk::CommandPool m_primary_command_pool;
		vk::CommandBuffer m_command_buffer;
	};
}