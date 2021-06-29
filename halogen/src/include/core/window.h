#ifndef WINDOW_H
#define WINDOW_H

#include "../configuration.h"
#include "../log.h"
#include "../core/input.h"

#include <SDL.h>

namespace halogen
{
    /* Base class for window creation -> current using SDL as windowing API. */
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