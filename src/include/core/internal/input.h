#pragma once

#include "../log.h"

#include <SDL.h>

namespace halogen
{
    /* Input mapping for keyboard and mouse. Consider putting them into separate functions later on. */
     namespace InputMap
    {
        struct KeyboardInput
        {
            enum Keys
            {
                W = SDL_SCANCODE_W,
                A = SDL_SCANCODE_A,
                S = SDL_SCANCODE_S,
                D = SDL_SCANCODE_D,

                Q = SDL_SCANCODE_Q,
                E = SDL_SCANCODE_E,

                Up = SDL_SCANCODE_UP,
                Down = SDL_SCANCODE_DOWN,
                Left = SDL_SCANCODE_LEFT,
                Right = SDL_SCANCODE_RIGHT,

                Quit = SDL_SCANCODE_ESCAPE,
                Enter = SDL_SCANCODE_RETURN
            };
        };

        struct MouseInput
        {
            enum MouseButtons
            {
                Left = SDL_BUTTON_LEFT,
                Right = SDL_BUTTON_RIGHT,
                Middle = SDL_BUTTON_MIDDLE
            };
        };
    };

     /* Input class has functions for processing inputs independent of the API used. */
    class Input
    {
    public:
        Input();
        ~Input();

        Input(const Input& other) = delete;

        void initialize_input();
        void process_inputs();

        bool is_key_pressed(InputMap::KeyboardInput::Keys key);
        bool quit();

    private:
        void clean_up();

    private:
        SDL_Event m_input_event;

        const Uint8 *m_keyboard_state;

        bool m_quit_application;
    };
}
