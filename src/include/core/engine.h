#ifndef ENGINE_H
#define ENGINE_H

#include "window.h"
#include "input.h"
#include "../log.h"
#include "platform.h"
#include "graphics/renderer.h"

#include <SDL.h>
#include <memory>

namespace halogen
{
    /* Makes it easier to implement multiple platforms in the future. */
    static std::unique_ptr<Platform> g_platform;

    /* Main engine class -> binds all other classes together. */
    class Engine
    {
    public:
        Engine();

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
        std::shared_ptr<Window> m_window;
        std::unique_ptr<Input> m_input;
        std::unique_ptr<Renderer> m_renderer;

        bool m_engine_quit;
    };
}
#endif
