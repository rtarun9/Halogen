#ifndef RENDERER_H
#define RENDERER_H

#include "../../../include/core/window.h"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <memory>
#include <vector>

namespace halogen
{
    class Renderer
    {
    public:
        Renderer();

        void initialize_renderer(std::shared_ptr<Window> window);

    private:
        VkInstance m_vulkan_instance;
    };
}
#endif