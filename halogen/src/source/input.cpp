#include "../include/core/input.h"

namespace halogen
{
    Input::Input()
    {
        m_current_pressed_keys.clear();

        m_quit_application = false;
    }

    void Input::reset_inputs()
    {
        for (size_t i = 0; i < m_current_pressed_keys.size(); i++)
        {
            m_current_pressed_keys.pop_back();
        }
    }

    void Input::process_inputs()
    {
        while (SDL_PollEvent(&m_input_event) != 0)
        {
            if (m_input_event.type == SDL_QUIT)
            {
                m_quit_application = true;
            }

            m_current_pressed_keys.push_back(m_input_event.key.keysym.scancode);
        }
    }

    bool Input::is_key_pressed(SDL_Scancode key)
    {
        for (SDL_Scancode indexed_key : m_current_pressed_keys)
        {
            if (indexed_key == key)
                return true;
        }

        return false;
    }

    bool Input::quit()
    {
        return m_quit_application || is_key_pressed(SDL_SCANCODE_ESCAPE);
    }

    Input::~Input()
    {}
}