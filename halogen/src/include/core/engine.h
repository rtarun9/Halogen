#ifndef ENGINE_H
#define ENGINE_H

#include "../../include/core/window.h"
#include "../../include/core/input.h"
#include "../../include/log.h"

#include <SDL.h>
#include <memory>

namespace halogen
{
    class Engine
    {
    public:
        Engine();

        void run();

        ~Engine();

    private:
        void initialize();

        void initialize_platform_backend();
        void close_platform_backend();

    private:
        std::unique_ptr<Window> m_window;
        Input m_input;

        bool m_quit_engine;
    };
}
#endif
