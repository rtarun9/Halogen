#include "../include/core/engine.h"

namespace halogen
{
    Engine::Engine()
    {
        m_window = nullptr;

        m_quit_engine = false;

        initialize();
    }

    void Engine::run()
    {
        while (!m_quit_engine)
        {
            m_input.process_inputs();

            if (m_input.quit() == true)
                return;

            m_input.reset_inputs();
        }
    }

    Engine::~Engine()
    {
        close_platform_backend();
    }

    void Engine::initialize()
    {
        initialize_platform_backend();

        m_window = std::make_unique<Window>(-1, -1, "SUCKS");
        m_window->create_window();
    }

    void Engine::initialize_platform_backend()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            assert::error("Failed to initialize platform backend SDL2");
        }
    }

    void Engine::close_platform_backend()
    {
        SDL_Quit();
    }
}