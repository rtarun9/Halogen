#include "../include/core/window.h"

namespace halogen
{
    Window::Window()
    {
        m_window = nullptr;

        m_window_width = configuration::DEFAULT_WINDOW_WIDTH;
        m_window_height = configuration::DEFAULT_WINDOW_HEIGHT;
        m_window_title = configuration::DEFAULT_WINDOW_TITLE;

        m_window_close = false;
    }

    Window::Window(int width, int height, const char *title)
    {
        m_window = nullptr;

        assert::error_condition(m_window_width < 0 || m_window_height < 0, "Cannot have window dimensions < 0");

        m_window_width = width;
        m_window_height = height;
        m_window_title = title;

        m_window_close = false;
    }

    void Window::create_window()
    {
        m_window = SDL_CreateWindow
        (
            m_window_title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            m_window_width, m_window_height,
            SDL_WINDOW_SHOWN
        );

        if (m_window == nullptr)
        {
            assert::error("Failed to create window. Platform : SDL2.");
        }
    }

    void Window::should_window_close(Input *input)
    {
        if (input->quit())
            m_window_close = true;
    }

    Window::~Window()
    {
        SDL_DestroyWindow(m_window);
    }
}