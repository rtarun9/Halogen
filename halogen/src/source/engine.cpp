#include "../include/core/engine.h"

namespace halogen
{
    Engine::Engine()
    {
        m_window = nullptr;
        m_engine_quit = false;
    }

    void Engine::start()
    {
        initialize();
        run();
    }

    void Engine::run()
    {
        while (!m_engine_quit)
        {
            m_input->process_inputs();

            if (m_input->quit() == true)
            {
                m_engine_quit = true;
            }

            if (m_input->is_key_pressed(InputMap::KeyboardInput::Enter))
            {
                debug::log("Enter key was pressed");
            }
        }
    }

    void Engine::initialize()
    {
        initialize_platform_backend();

        m_window = std::make_unique<Window>();
        m_window->create_window();

        m_input = std::make_unique<Input>();

    }

    void Engine::initialize_platform_backend()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            debug::error("Failed to initialize platform backend SDL2");
        }
    }

    void Engine::close_platform_backend()
    {
        SDL_Quit();
    }

    void Engine::clean_up()
    {
        close_platform_backend();
        debug::log("Cleaning up engine.");
    }


    Engine::~Engine()
    {
        clean_up();
    }

}