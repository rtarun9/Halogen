#include "../../include/core/graphics/vk_mesh.h"

namespace halogen
{
    //Vertex input description : attributes + description
    VertexInputDescription Vertex::get_vertex_input_description()
    {
        //Currently interleaving data (basically all attributes are bound to a single binding).
        //Not creating individual bindings for each attribute for now.
        VertexInputDescription vertex_input_description = {};

        VkVertexInputBindingDescription main_binding_description = {};
        main_binding_description.binding = 0;

        //Interval of bytes of one attribute to the next. Since interleaving data, sizeof (Vertex) is fine.
        main_binding_description.stride = sizeof(Vertex);
        main_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        //Vertex Attribute : input variable specified on a per - vertex basis
        //Location needs to be the same as that in VERTEX SHADER.

        VkVertexInputAttributeDescription position_attribute_description = {};
        position_attribute_description.binding = 0;
        position_attribute_description.location = 0;
        position_attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;
        position_attribute_description.offset = offsetof(Vertex, m_position);

        VkVertexInputAttributeDescription normal_attribute_description = {};
        normal_attribute_description.binding = 0;
        normal_attribute_description.location = 1;
        normal_attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;
        normal_attribute_description.offset = offsetof(Vertex, m_normal);

        VkVertexInputAttributeDescription color_attribute_description = {};
        color_attribute_description.binding = 0;
        color_attribute_description.location = 2;
        color_attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;
        color_attribute_description.offset = offsetof(Vertex, m_color);

        vertex_input_description.m_attribute_descriptions.push_back(position_attribute_description);
        vertex_input_description.m_attribute_descriptions.push_back(normal_attribute_description);
        vertex_input_description.m_attribute_descriptions.push_back(color_attribute_description);

        vertex_input_description.m_binding_descriptions.push_back(main_binding_description);

        return vertex_input_description;
    }

    //Heavily referenced tinyobjloader's example code (for the deprecated Api).
    //Todo : Switch to the newer OOP api
    void Mesh::load_obj_from_file(std::basic_string<char> obj_file_path)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warning;
		std::string error;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, obj_file_path.c_str());

		if (!warning.empty())
		{
			debug::warning("tinyobj warning : ", warning);
		}

		if (!error.empty())
		{
			debug::warning("tinyobj error : ", error);
		}

		if (!ret)
		{
			debug::error("Failed to load OBJ file from path : ", obj_file_path.c_str());
		}

		//Loop over all shapes
		for (size_t s = 0; s < shapes.size(); s++)
		{
			//Loop over faces (polygons)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
			{
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
				//Remove if you want to load more than triangles
				fv = 3;

				for (size_t v = 0; v < fv; v++)
				{
					//Access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					//Vertex positions
					tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
					tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
					tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

					//Vertex normals
					tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
					tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
					tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

					//Copy into custom vertex  class
					Vertex vertex;
					vertex.m_position.x = vx;
					vertex.m_position.y = -vy;
					vertex.m_position.z= vz;

					vertex.m_normal.x = nx;
					vertex.m_normal.y = ny;
					vertex.m_normal.z = nz;

					vertex.m_color = vertex.m_normal;

					m_vertices.push_back(vertex);
				}

				index_offset += fv;
			}
		}
	}
}

