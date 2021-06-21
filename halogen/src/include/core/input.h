#ifndef INPUT_H
#define INPUT_H

#include "../../include/log.h"

#include <SDL.h>
#include <vector>
#include <string>
#include <map>

namespace halogen
{
    class Input
    {
    public:
        Input();

        void process_inputs();

        bool is_key_pressed(std::string key);
        bool quit();

        ~Input();

    private:
        void set_input_map();
        void clean_up();

    private:
        SDL_Event m_input_event;

        std::map<std::string, SDL_Scancode> m_keys;
        const Uint8 *m_keyboard_state;

        bool m_quit_application;
    };
}
#endif