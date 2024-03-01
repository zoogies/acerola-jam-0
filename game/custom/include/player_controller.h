
#ifndef PLAYER_CONTROLLER
#define PLAYER_CONTROLLER

#include <yoyoengine/yoyoengine.h>

struct bind_state_data {
    /*
        Keybinds that control how we move
    */
    SDL_Keycode bind_left;
    SDL_Keycode bind_right;
    SDL_Keycode bind_up;
    SDL_Keycode bind_down;

    /*
        State machine
    */
    bool moving_left;
    bool moving_right;
    bool moving_up;
    bool moving_down;
};

void init_player_controller();

void shutdown_player_controller();

void player_input_handler(SDL_Event e);

void player_controller_additional_render();

void player_controller_pre_frame();

void player_controller_post_frame();

#endif