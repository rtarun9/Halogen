#pragma once

#include "types.h"

#include <glm/glm.hpp>

namespace halo
{
	// hold information on how the vertex buffer data is laid out and how to be interpreted.
	struct VertexInputLayoutDescription
	{
		// contains how data is binded, and its vertex rate
		std::vector<VkVertexInputBindingDescription> m_bindings;

		// attributes contain info such as the location to which a particular piece of data is stored (accessed via layout (location = x) in vertex shader
		std::vector<VkVertexInputAttributeDescription> m_attributes;
	};

	struct Vertex
	{
		glm::vec3 m_position;
		glm::vec3 m_normal;
		glm::vec3 m_color;

		[[nodiscard]]
		static VertexInputLayoutDescription get_vertex_input_layout_description();
	};

	struct Mesh
	{
		std::vector<Vertex> m_vertices;
		AllocatedBuffer m_allocated_buffer;

		void load_obj_file(const char *file_path);
	};

	struct PushConstants
	{
		glm::mat4 m_transform_mat;
	};
}
