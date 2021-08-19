#include "../../include/core/game/scene.h"

namespace halogen
{
	Scene::Scene(): m_scene_initialized(false)
	{
		m_meshes.clear();
		m_game_objects.clear();
		m_materials.clear();
	}

	Scene::~Scene()
	{
		m_scene_initialized = false;

		m_meshes.clear();
		m_game_objects.clear();
		m_meshes.clear();
	}

	Material *Scene::create_material(VkPipeline pipeline, VkPipelineLayout pipeline_layout, const std::string &name)
	{
		Material new_mat = {};
		new_mat.m_pipeline = pipeline;
		new_mat.m_pipeline_layout = pipeline_layout;
		m_materials[name] = new_mat;

		return &m_materials[name];
	}

	Material *Scene::get_material(const std::string &name)
	{
		auto it = m_materials.find(name);
		if (it == m_materials.end())
		{
			debug::warning("Could not find material with name : ", name);
			return nullptr;
		}
		else
		{
			return &it->second;
		}
	}

	Mesh *Scene::get_mesh(const std::string &name)
	{
		auto it = m_meshes.find(name);
		if (it == m_meshes.end())
		{
			debug::warning("Could not find mesh with name : ", name);
			return nullptr;
		}
		else
		{
			return &it->second;
		}
	}

	void Scene::render_objects(VkCommandBuffer command_buffer, std::vector<GameObject> &game_objects)
	{
		glm::vec3 camera_position = m_camera.m_camera_position;

		glm::mat4 view_mat = glm::mat4(1.0f);
		view_mat = glm::translate(glm::mat4(1.0f), camera_position);

		glm::mat4 projection = glm::perspective(glm::radians(75.0f), 1080.0f / 720.0f, 0.1f, 100.0f);

		Mesh *last_mesh = nullptr;
		Material *last_material = nullptr;

		for (int i = 0; i < m_game_objects.size(); i++)
		{
			GameObject& object = m_game_objects[i];

			if (object.m_material != last_material)
			{
				vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, object.m_material->m_pipeline);
				last_material = object.m_material;
			}

			glm::mat4 model_mat = object.m_transformation_mat;

			glm::mat4 mvp_matrix = projection * view_mat * model_mat;

			MeshPushConstants push_constants = {};
			push_constants.m_transform_matrix = mvp_matrix;

			vkCmdPushConstants(command_buffer, object.m_material->m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &push_constants);

			if (object.m_mesh != last_mesh)
			{
				VkDeviceSize offset = 0;
				vkCmdBindVertexBuffers(command_buffer, 0, 1, &object.m_mesh->m_vertex_buffer.m_buffer, &offset);
				last_mesh = object.m_mesh;
			}

			vkCmdDraw(command_buffer, object.m_mesh->m_vertices.size(), 1, 0, 0);
		}
	}

	bool Scene::is_scene_initialzed() const
	{
		return m_scene_initialized;
	}
}