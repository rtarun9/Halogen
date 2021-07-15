#ifndef RENDERER_H
#define RENDERER_H

#include "../../configuration.h"
#include "instance.h"
#include "device.h"
#include "surface.h"
#include "vk_debug.h"
#include "swapchain.h"
#include "pipeline.h"
#include "../../log.h"
#include "../window.h"

#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>
#include <vector>
#include <iostream>
#include <string>
#include <memory>

namespace halogen
{
    /* Base class for the renderer that ties up all wrapper vulkan / scene classes. */
    class Renderer
    {
    public:
        Renderer();

        void initialize_renderer(std::shared_ptr<Window>& window);
        void render();

        ~Renderer();

    private:

    private:
        std::unique_ptr<Instance> m_instance;
        std::unique_ptr<Device> m_device;
        std::unique_ptr<Surface> m_surface;
        std::unique_ptr<Swapchain> m_swapchain;
        std::unique_ptr<Pipeline> m_pipeline;
    };
}
#endif