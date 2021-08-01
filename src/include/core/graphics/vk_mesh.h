#pragma once

#include "vk_types.h"

#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <glm/glm.hpp>

namespace halogen
{
    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription> m_binding_description;
        std::vector<VkVertexInputAttributeDescription> m_attribute_description;

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
        AllocatedBuffer m_allocated_buffer;
    };
}

