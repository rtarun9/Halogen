#include "window.h"

namespace halogen
{
    Window::Window() : m_window(nullptr), m_window_height(configuration::DEFAULT_WINDOW_HEIGHT), m_window_width(configuration::DEFAULT_WINDOW_WIDTH), m_window_title(configuration::DEFAULT_WINDOW_TITLE)
    {
        create_window();
    }

    Window::Window(int width, int height, const char *title) : m_window(nullptr), m_window_height(height), m_window_width(width), m_window_title(title)
    {
        assert(m_window_width < 0 || m_window_height < 0, "Cannot have window dimensions < 0");
        create_window();
    }

    Window::~Window()
    {
        close();
    }

    void Window::create_window()
    {
        int sdl_init_status = SDL_WasInit(SDL_INIT_EVERYTHING);
        if (!(sdl_init_status & SDL_INIT_EVERYTHING))
        {
            debug::error("Trying to create Window without initializing SDL.");
        }

        m_window = SDL_CreateWindow
        (
            m_window_title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            m_window_width, m_window_height,
            SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN
        );

        if (m_window == nullptr)
        {
            debug::error("Failed to create window.", "Platform : SDL2.");
        }

        debug::log("Created window");
    }

    void Window::close()
    {
        m_window = nullptr;
        m_window_title = nullptr;
        SDL_DestroyWindow(m_window);

        debug::log("Cleaning up window.");
    }

    const SDL_Window& Window::get_window() const
    {
        assert(m_window == nullptr, "Cannot return reference to window since window is null.");
        return *m_window;
    }
}