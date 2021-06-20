#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>
#include <vector>

namespace halogen
{
    class Input
    {
    public:
        Input();

        void reset_inputs();
        void process_inputs();

        bool is_key_pressed(SDL_Scancode key);
        bool quit();

        ~Input();

    private:
        SDL_Event m_input_event;

        std::vector<SDL_Scancode> m_current_pressed_keys;
        bool m_quit_application;
    };
}
#endif