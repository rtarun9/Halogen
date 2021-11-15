#include "../include/mesh.h"

#include <tiny_obj_loader.h>

#include <string>
#include <iostream>

namespace halo
{
	VertexInputLayoutDescription Vertex::get_vertex_input_layout_description()
	{
		VertexInputLayoutDescription input_layout_desc = {};

		// one main vertex binding
		vk::VertexInputBindingDescription m_primary_binding_desc = {};
		m_primary_binding_desc.inputRate = vk::VertexInputRate::eVertex;
		m_primary_binding_desc.stride = sizeof(Vertex);
		m_primary_binding_desc.binding = 0;

		input_layout_desc.m_bindings.push_back(m_primary_binding_desc);

		// attribute "position" at location 0
		vk::VertexInputAttributeDescription position_attribute_desc = {};
		position_attribute_desc.binding  = 0;
		position_attribute_desc.format = vk::Format::eR32G32B32Sfloat;
		position_attribute_desc.location = 0;
		position_attribute_desc.offset = offsetof(Vertex, m_position);

		// attribute normal at location 1
		vk::VertexInputAttributeDescription normal_attribute_desc = {};
		normal_attribute_desc.binding = 0;
		normal_attribute_desc.format = vk::Format::eR32G32B32Sfloat;
		normal_attribute_desc.location = 1;
		normal_attribute_desc.offset  = offsetof(Vertex, m_normal);

		// attribute color at location 2
		vk::VertexInputAttributeDescription color_attribute_desc = {};
		color_attribute_desc.binding = 0;
		color_attribute_desc.format = vk::Format::eR32G32B32Sfloat;
		color_attribute_desc.location = 2;
		color_attribute_desc.offset = offsetof(Vertex, m_color);

		input_layout_desc.m_attributes.clear();
		input_layout_desc.m_attributes.push_back(position_attribute_desc);
		input_layout_desc.m_attributes.push_back(normal_attribute_desc);
		input_layout_desc.m_attributes.push_back(color_attribute_desc);

		return input_layout_desc;
	}

	void Mesh::load_obj_from_file(const char* file_path)
	{
		// code from the example code (new oop based api) from tinyobjloader's github : https://github.com/tinyobjloader/tinyobjloader
		tinyobj::ObjReaderConfig reader_config;
		reader_config.mtl_search_path = "./";
		
		tinyobj::ObjReader reader;
		if (!reader.ParseFromFile(std::string(file_path), reader_config))
		{
			if (!reader.Error().empty())
			{
				throw std::runtime_error(reader.Error());
			}
		}

		if (!reader.Warning().empty())
		{
			std::cout << "TinyObjReader : " << reader.Warning();
		}

		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();
		auto& materials = reader.GetMaterials();

		// loop over all shapes
		for (size_t s = 0; s < shapes.size(); s++)
		{
			// loop over faces (polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
			{
				size_t fv = static_cast<size_t>(shapes[s].mesh.num_face_vertices[f]);

				// loop over vertices in face
				for (size_t v = 0; v < fv; v++)
				{
					// acess to vertex
					tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];

					tinyobj::real_t vx = attrib.vertices[3 * size_t(index.vertex_index) + 0];
					tinyobj::real_t vy = attrib.vertices[3 * size_t(index.vertex_index) + 1];
					tinyobj::real_t vz = attrib.vertices[3 * size_t(index.vertex_index) + 2];

					tinyobj::real_t nx = attrib.normals[3 * size_t(index.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(index.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(index.normal_index) + 2];
			
					tinyobj::real_t cr = attrib.colors[3 *size_t(index.vertex_index) + 0];
					tinyobj::real_t cg = attrib.colors[3 *size_t(index.vertex_index) + 1];
					tinyobj::real_t cb = attrib.colors[3 *size_t(index.vertex_index) + 2];
					
					Vertex vertex;
					vertex.m_position.x = vx;
					vertex.m_position.y = vy;
					vertex.m_position.z = vz;

					vertex.m_normal.x = nx;
					vertex.m_normal.y = ny;
					vertex.m_normal.z = nz;

					// note : setting colors to be normals for debuggin purposes.
					vertex.m_color.r = nx;
					vertex.m_color.g = ny;
					vertex.m_color.b = nz;

					m_vertices.push_back(vertex);
				}

				index_offset += fv;

				shapes[s].mesh.material_ids[f];
			}
		}
	}
}