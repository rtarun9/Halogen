#pragma once

#include <vulkan/vulkan.hpp>

// need to remove this after refactoring is done
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <glm/glm.hpp>

#include <vector>
#include <functional>
#include <iostream>

namespace halo
{
	struct Mesh;

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

	// struct for the buffer allocated + some information like where it has been allocated, heap type, and VMA state.
	struct AllocatedBuffer
	{
		VkBuffer m_buffer;
		VmaAllocation m_allocation_data;
	};

	// struct for image allocated
	struct AllocatedImage
	{
		VkImage m_image;
		VmaAllocation m_allocation_data;
	};

	// struct for game objects :  

	struct Material
	{
		VkPipeline m_pipeline;
		VkPipelineLayout m_pipeline_layout;
	};

	struct GameObject
	{
		Mesh *m_mesh;
		Material *m_material;
		glm::mat4 m_mesh_transform;
	};

}