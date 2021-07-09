#include "../include/core/input.h"

namespace halogen
{
    Input::Input()
    {
        m_quit_application = false;
    }

    void Input::process_inputs()
    {
        if (SDL_PollEvent(&m_input_event) != 0)
        {
            m_keyboard_state = SDL_GetKeyboardState(nullptr);

            if (m_input_event.type == SDL_QUIT)
            {
                m_quit_application = true;
            }
        }
    }

    bool Input::is_key_pressed(InputMap::KeyboardInput::Keys key)
    {
        if (m_keyboard_state[key] == 1)
            return true;

        return false;
    }

    bool Input::quit()
    {
        return m_quit_application || is_key_pressed(InputMap::KeyboardInput::Quit);
    }

    void Input::clean_up()
    {
        debug::log("Cleaning up input.");
    }

    Input::~Input()
    {
        clean_up();
    }
}