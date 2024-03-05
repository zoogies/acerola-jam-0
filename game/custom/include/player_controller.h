
#ifndef PLAYER_CONTROLLER
#define PLAYER_CONTROLLER

#include <stdbool.h>
#include <yoyoengine/yoyoengine.h>

struct bind_state_data {
    /*
        Keybinds that control how we move
    */
    SDL_Keycode bind_left;
    SDL_Keycode bind_right;
    SDL_Keycode bind_up;
    SDL_Keycode bind_down;
    SDL_Keycode bind_attack;

    /*
        State machine
    */
    bool moving_left;
    bool moving_right;
    bool moving_up;
    bool moving_down;
    bool attacking;

    bool discovered_left_bind;
    bool discovered_right_bind;
    bool discovered_up_bind;
    bool discovered_down_bind;
    bool discovered_attack_bind;

    bool discovered_bind_this_frame;
};

extern bool player_controller_active;

void init_player_controller();

void shutdown_player_controller();

void player_input_handler(SDL_Event e);

void player_controller_additional_render();

void player_controller_pre_frame();

void player_controller_post_frame();

void player_controller_trigger_handler(struct ye_entity *e1, struct ye_entity *e2);

#endif