#pragma once

#include "window.h"
#include "input.h"
#include "platform.h"

#include "../configuration.h"
#include "../log.h"

#include "../graphics/renderer.h"

#include <SDL.h>
#include <memory>

namespace halogen
{
    /* Main engine class -> binds all other classes together. */
    class Engine
    {
    public:
        Engine(Config config);

        Engine(const Engine& other) = delete;
        Engine& operator=(const Engine& other) = delete;

        void start();
        void run();

        ~Engine();

    private:
        void initialize();

        void update();
        void process_input();
        void render();

        void clean_up();

    private:
        Platform m_platform;

        Window m_window {1080, 720, "Halogen"};
        Input m_input;
        Renderer m_renderer{m_input};

        bool m_engine_quit;
    };
}
