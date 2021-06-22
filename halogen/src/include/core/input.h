#ifndef INPUT_H
#define INPUT_H

#include "../../include/log.h"

#include <SDL.h>
#include <vector>
#include <string>
#include <map>

namespace halogen
{
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

    class Input
    {
    public:
        Input();

        void process_inputs();

        bool is_key_pressed(InputMap::KeyboardInput::Keys key);
        bool quit();

        ~Input();

    private:
        void clean_up();

    private:
        SDL_Event m_input_event;

        const Uint8 *m_keyboard_state;

        bool m_quit_application;
    };
}
#endif