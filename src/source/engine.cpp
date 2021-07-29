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

        //Press 'A' to switch between pipelines.
        //Not focussing on clean code here since this is just a tester.
        if (m_input.is_key_pressed(InputMap::KeyboardInput::A))
        {
            m_selected_pipeline = 0;
        }
        else
        {
            m_selected_pipeline = 1;
        }
    }

    void Engine::update()
    {
        //physics, fixed framerate, etc.
    }

    void Engine::render()
    {
        m_renderer.render(m_selected_pipeline);
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