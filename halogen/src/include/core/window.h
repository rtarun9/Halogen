#ifndef WINDOW_H
#define WINDOW_H

#include "../../include/core/configuration.h"
#include "../../include/log.h"
#include "../../include/core/input.h"

#include <SDL.h>

namespace halogen
{
    class Window
    {
    public:
        Window();
        Window(int width, int height, const char *title);

        void create_window();
        void close();

        SDL_Window& get_window();

        ~Window();

    private:

        SDL_Window *m_window;

        int m_window_height;
        int m_window_width;
        const char *m_window_title;

        bool m_window_close;
    };
}

#endif