#ifndef ENGINE_H
#define ENGINE_H

#include "../../include/core/window.h"
#include "../../include/core/input.h"
#include "../../include/log.h"
#include "../../include/core/platform.h"
#include "../../include/core/graphics/renderer.h"

#include <SDL.h>
#include <memory>

namespace halogen
{
    /* Makes it easier to implement multiple platforms in the future. */
    static std::unique_ptr<Platform> g_platform;

    class Engine
    {
    public:
        Engine();

        void start();
        void run();

        ~Engine();

    private:
        void initialize();
        void clean_up();

    private:
        std::shared_ptr<Window> m_window;
        std::unique_ptr<Input> m_input;
        std::unique_ptr<Renderer> m_renderer;
        bool m_engine_quit;
    };
}
#endif
