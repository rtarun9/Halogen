#pragma once

#include "../configuration.h"
#include "../log.h"

#include <SDL.h>

namespace halogen
{
    /* Base class for window creation. */
    class Window
    {
    public:
        Window();
        Window(int width, int height, const char *title);
        ~Window();

        Window(const Window& other) = delete;
        Window& operator=(const Window& other) = delete;

        const SDL_Window& get_window() const;

    private:
        void create_window();
        void close();

    private:
        SDL_Window *m_window;

        int m_height;
        int m_width;
        const char *m_title;
    };
}

