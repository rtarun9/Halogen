#include "../include/core/engine.h"

namespace halogen
{
    Engine::Engine()
    {
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
            process_input();
            update();
            render();
        }
    }

    void Engine::initialize()
    {
        m_renderer.initialize_renderer(m_window);
    }

    void Engine::process_input()
    {
        m_input.process_inputs();

        if (m_input.quit())
        {
            m_engine_quit = true;
        }

    }

    void Engine::update()
    {
        //physics, fixed framerate, etc.
    }

    void Engine::render()
    {
        m_renderer.render();
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