#pragma once

#include "../math/vector3.h"
#include "../math/vector4.h"
#include "vk_types.h"

#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

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
        math::Vector3 m_position;
        math::Vector3 m_normal;
        math::Vector3 m_color;

        static VertexInputDescription get_vertex_input_description();
    };

    struct Mesh
    {
        std::vector<Vertex> m_vertices;
        AllocatedBuffer m_vertex_buffer;
    };

	//Load some simple data by passing into command buffer.
    struct MeshPushConstants
    {
    	alignas(16) math::Vector3 m_offset;
    };
}

