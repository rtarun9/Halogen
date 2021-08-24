#pragma once

#include "../graphics/vk_mesh.h"
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

using id_t = uint32_t;

namespace halogen
{
	struct Material
	{
		VkPipeline m_pipeline;
		VkPipelineLayout m_pipeline_layout;
	};

	class GameObject
	{
	public:
		GameObject();
		~GameObject();

		uint32_t get_id();

	private:
		uint32_t create_game_object();

	public:
		Mesh *m_mesh;
		Material *m_material;
		glm::mat4 m_transformation_mat;

		uint32_t m_id;
	};
}