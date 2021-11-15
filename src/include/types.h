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

	// used for vulkan cleanup. Function calls are invoked in reverse order as what they are pushed in, resulting in the latest function to be pushed to be called first.
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

	// temporary struct, that holds transform matrix of each game object. Data sent to the shader via push constants.
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

	// struct for image allocated + some allocation details.
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

	// GameObject : contains a transform matrix (sent to shader via PUSH constants), mesh, and material
	struct GameObject
	{
		Mesh *m_mesh;
		Material *m_material;
		glm::mat4 m_mesh_transform;
	};

	// Buffer for camera details (will be sent to GPU via descriptor sets)
	struct CameraData
	{
		glm::mat4 m_view_mat;
		glm::mat4 m_projection_mat;

		// to avoid having to multiply this in shaders (since its constant for each vertex);
		glm::mat4 m_projection_view_mat;
	};

	// FrameData : includes vulkan handles that are unique per frame (one frame is using GPU, the other is using CPU). Needed for double buffering.
	// When GPU is rendering frame N, CPU does work on the frame N + 1
	struct FrameData
	{
		vk::Semaphore m_presentation_semaphore;
		vk::Semaphore m_render_semaphore;

		vk::Fence m_render_fence;

		vk::CommandPool m_primary_command_pool;
		vk::CommandBuffer m_command_buffer;

		// each frame has its own allocated buffer so that there will be no issues in overlapping data (because of double buffering)
		AllocatedBuffer m_camera_allocated_buffer;

		vk::DescriptorSet m_global_descriptor_set;
	};

	// struct having data of environment. Size : 4 * 4 * 5  = 80 bytes
	struct EnvironmentData
	{
		// w is used as exponent
		glm::vec4 m_fog_color;

		// using x as minimum distance, and y as maximum distance
		glm::vec4 m_fog_distance;
		glm::vec4 m_ambient_color;

		// w is used as power
		glm::vec4 m_sunlight_direction;
		glm::vec4 m_sunlight_color;
	};
}