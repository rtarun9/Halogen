#pragma once

#include "game_object.h"
#include "camera.h"

#include "../log.h"

#include "../graphics/vk_mesh.h"

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace halogen
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		Material *get_material(const std::string& name);
		Mesh *get_mesh(const std::string& name);

		Material *create_material(VkPipeline pipeline, VkPipelineLayout pipeline_layout, const std::string& name);

		void render_objects(VkCommandBuffer command_buffer, std::vector<GameObject>& game_objects);

		bool is_scene_initialzed() const;

	public:
		bool m_scene_initialized;

	public:
		std::vector<GameObject> m_game_objects;
		std::unordered_map<std::string, Material> m_materials;
		std::unordered_map<std::string, Mesh> m_meshes;

		Camera m_camera;
	};
}