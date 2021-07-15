#include "../../include/core/graphics/renderer.h"

namespace halogen
{
    Renderer::Renderer() : m_instance(nullptr), m_device(nullptr), m_surface(nullptr)
    {
    }

        void Renderer::initialize_renderer(std::shared_ptr<Window>& window)
        {
            m_instance = std::make_unique<Instance>();
            m_surface = std::make_unique<Surface>(m_instance->get_vulkan_instance(), window->get_window());

            m_device = std::make_unique<Device>(m_instance->get_vulkan_instance(), m_surface->get_window_surface());
            m_swapchain = std::make_unique<Swapchain>(m_instance->get_vulkan_instance(), m_device->get_device(), m_device->get_queue_family_indices(), m_surface->get_window_surface());
            m_pipeline = std::make_unique<Pipeline>(m_instance->get_vulkan_instance(), m_device->get_device());
        }

    void Renderer::render()
    {

    }

    Renderer::~Renderer()
    {
    }
}