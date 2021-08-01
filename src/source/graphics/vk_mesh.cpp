#include "../../include/core/graphics/vk_mesh.h"

namespace halogen
{
    VertexInputDescription Vertex::get_vertex_input_description()
    {
        VertexInputDescription vertex_input_description = {};

        VkVertexInputBindingDescription main_binding_description = {};
        main_binding_description.binding = 0;
        main_binding_description.stride = sizeof(Vertex);
        main_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

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

        vertex_input_description.m_attribute_description.push_back(position_attribute_description);
        vertex_input_description.m_attribute_description.push_back(normal_attribute_description);
        vertex_input_description.m_attribute_description.push_back(color_attribute_description);

        vertex_input_description.m_binding_description.push_back(main_binding_description);

        return vertex_input_description;
    }
}