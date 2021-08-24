#pragma once

#include "../math/vector3.h"
#include "../math/vector4.h"


#include "../log.h"

#include "vk_types.h"
#include "../math/matrix4x4.h"

#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <tiny_obj_loader.h>
#include <glm/glm.hpp>

namespace halogen
{
    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription> m_binding_descriptions;
        std::vector<VkVertexInputAttributeDescription> m_attribute_descriptions;

        VkPipelineVertexInputStateCreateFlags m_flags = 0;
    };

    struct Vertex
    {
        glm::vec3 m_position;
        glm::vec3 m_normal;
        glm::vec3 m_color;

        static VertexInputDescription get_vertex_input_description();
    };

    struct Mesh
    {
        std::vector<Vertex> m_vertices;
        AllocatedBuffer m_vertex_buffer;

        void load_obj_from_file(std::basic_string<char> obj_file_path);
    };

	//Load some simple data by passing into command buffer.
    struct MeshPushConstants
    {
    	alignas(16) glm::mat4 m_transform_matrix;
    };
}

