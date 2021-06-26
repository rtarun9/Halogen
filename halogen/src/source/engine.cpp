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

            if (m_input->quit())
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
        g_platform = std::make_unique<Platform>();
        g_platform->initialize_backend();

        m_window = std::make_shared<Window>();
        m_window->create_window();

        m_input = std::make_unique<Input>();

        m_renderer = std::make_unique<Renderer>(m_window);
        m_renderer->initialize_renderer();
    }

    void Engine::clean_up()
    {
        debug::log("Cleaning up engine.");
    }


    Engine::~Engine()
    {
        clean_up();
    }

}