#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace halogen
{
    //Basic struct for holding allocated buffer and its data
    struct AllocatedBuffer
    {
        //Vulkan GPU side buffer.
        VkBuffer m_buffer;

        //If vma wasn't used, this would be there
        //VkDeviceMemory m_vertex_buffer_memory;

        //Hold state VMA uses (size, memory buffer was allocated from).
        VmaAllocation m_allocation;
    };

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
        AllocatedBuffer m_allocated_buffer;
    };

    struct MeshPushConstants
    {
        glm::vec4 m_data;
        glm::mat4 m_render_matrix;
    };
}

