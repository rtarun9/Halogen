#include "../include/vk_mesh.h"

namespace halo
{
	VertexInputLayoutDescription Vertex::get_vertex_input_layout_description()
	{
		VertexInputLayoutDescription input_layout_desc = {};

		// one main vertex binding
		VkVertexInputBindingDescription m_primary_binding_desc = {};
		m_primary_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_primary_binding_desc.stride = sizeof(Vertex);
		m_primary_binding_desc.binding = 0;

		input_layout_desc.m_bindings.push_back(m_primary_binding_desc);

		// attribute "position" at location 0

		VkVertexInputAttributeDescription position_attribute_desc = {};
		position_attribute_desc.binding  = 0;
		position_attribute_desc.format = VK_FORMAT_R32G32B32_SFLOAT;
		position_attribute_desc.location = 0;
		position_attribute_desc.offset = offsetof(Vertex, m_position);

		// attribute normal at location 1
		VkVertexInputAttributeDescription normal_attribute_desc = {};
		normal_attribute_desc.binding = 0;
		normal_attribute_desc.format = VK_FORMAT_R32G32B32_SFLOAT;
		normal_attribute_desc.location = 1;
		normal_attribute_desc.offset  = offsetof(Vertex, m_normal);

		// attribute color at location 2
		VkVertexInputAttributeDescription color_attribute_desc = {};
		color_attribute_desc.binding = 0;
		color_attribute_desc.format = VK_FORMAT_R32G32B32_SFLOAT;
		color_attribute_desc.location = 2;
		color_attribute_desc.offset = offsetof(Vertex, m_color);

		input_layout_desc.m_attributes.clear();
		input_layout_desc.m_attributes.push_back(position_attribute_desc);
		input_layout_desc.m_attributes.push_back(normal_attribute_desc);
		input_layout_desc.m_attributes.push_back(color_attribute_desc);

		return input_layout_desc;
	}
}