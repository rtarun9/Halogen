#ifndef RENDERER_H
#define RENDERER_H

#include "../../../include/core/window.h"
#include "../../../include/core/platform.h"
#include "../../../include/core/configuration.h"
#include "../../../include/core/graphics/vk_utils.h"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <memory>
#include <vector>

namespace halogen
{
    class Renderer
    {
    public:
        Renderer(std::shared_ptr<Window> &window);

        void initialize_renderer();

        void cleanup();

    private:
        void initialize_vulkan();
        void create_vulkan_instance();
        void setup_debug_messenger();

        bool check_validation_layer_support();

    private:
        vk::Instance m_vulkan_instance;

        /*NOTE : To be removed later : only used temporarily*/
        std::shared_ptr<Window>& m_window;

        vk::DebugUtilsMessengerEXT m_debug_messenger;
    };
}
#endif