
#include <yoyoengine/yoyoengine.h>
#include <math.h>
// hack to disable error (at compile time this is defined by libm):
#ifndef M_PI
#define M_PI 3.14
#endif

bool player_moved_this_frame;

int mx = 0;
int my = 0;
int px = 0;
int py = 0;

struct ye_entity * player_ent = NULL;

/*
    Keybinds that control how we move
*/
SDL_Keycode bind_left;
SDL_Keycode bind_right;
SDL_Keycode bind_up;
SDL_Keycode bind_down;

bool moving_left = false;
bool moving_right = false;
bool moving_up = false;
bool moving_down = false;

void init_player_controller(){
    player_ent = ye_get_entity_by_name("PLAYER");

    bind_left = SDLK_a;
    bind_right = SDLK_d;
    bind_up = SDLK_w;
    bind_down = SDLK_s;
}

void shutdown_player_controller(){
    player_ent = NULL;
}

void player_controller_additional_render(){
    // struct ye_rectf cpos = ye_get_position(YE_STATE.engine.target_camera,YE_COMPONENT_CAMERA);

    // printf("%d,%d,%d,%d\n",px - (cpos.x/2),py - (cpos.y/2),mx,my);
    // // debug - draw a line from the player center to the mouse
    // SDL_SetRenderDrawBlendMode(YE_STATE.runtime.renderer, SDL_BLENDMODE_BLEND);
    // SDL_SetRenderDrawColor(YE_STATE.runtime.renderer, 255, 0, 0, 255);
    // SDL_RenderDrawLine(YE_STATE.runtime.renderer, px - (cpos.x/2),py - (cpos.y/2), mx, my);
    // SDL_SetRenderDrawBlendMode(YE_STATE.runtime.renderer, SDL_BLENDMODE_NONE);
}

/*
    We get many events per frame, so we just flag if we are doing an action to let it execute once on post frame
*/
void player_bind_abstraction(SDL_Event e){
    // if(e.type == SDL_KEYDOWN){
    //     if(e.key.keysym.sym == bind_left){
    //         printf("pressed left bind\n");
    //         // player_ent->physics->velocity.x = -10;
    //         player_moved_this_frame = true;
    //         moving_left = true;
    //     }
    //     else{
    //         moving_left = false;
    //     }
    //     if(e.key.keysym.sym == bind_right){
    //         printf("pressed right bind\n");
    //         // player_ent->physics->velocity.x = 10;
    //         player_moved_this_frame = true;
    //         moving_right = true;
    //     }
    //     else{
    //         moving_right = false;
    //     }
    //     if(e.key.keysym.sym == bind_up){
    //         printf("pressed up bind\n");
    //         // player_ent->physics->velocity.y = -10;
    //         player_moved_this_frame = true;
    //         moving_up = true;
    //     }
    //     else{
    //         moving_up = false;
    //     }
    //     if(e.key.keysym.sym == bind_down){
    //         printf("pressed down bind\n");
    //         // player_ent->physics->velocity.y = 10;
    //         player_moved_this_frame = true;
    //         moving_down = true;
    //     }
    //     else{
    //         moving_down = false;
    //     }
    // }
    // else{
    //     moving_up = false;
    //     moving_down = false;
    //     moving_left = false;
    //     moving_right = false;
    // }
}

void player_input_handler(SDL_Event e){
    /*
        Update mouse position (used for character looking at mouse)
    */
    if(e.type == SDL_MOUSEMOTION){
        SDL_GetMouseState(&mx, &my);
        ye_get_mouse_world_position(&mx, &my);
    }

    /*
        Update player position
    */
    struct ye_rectf pos = ye_get_position(player_ent, YE_COMPONENT_RENDERER);
    px = pos.x + (pos.w / 2);
    py = pos.y + (pos.h / 2);


    /*
        Change rotation to look at mouse
    */
    int dx = mx - px;
    int dy = my - py;
    double angle = atan2(dy,dx) * (180.0 / M_PI) + 90;
    player_ent->renderer->rotation = (float)angle;

    /*
        Handle movement
    */
    player_bind_abstraction(e);

    // TODO: optimization, do this once a frame instead of once an event (can be many in a frame)
}

/*
    Here we will route through custom input handler and then into the player controller,
    use some kind of state machine to define up down left right and actions, then we just have an 
    input layer abstracting those to the changing keybinds
*/

void player_controller_pre_frame(){
    // reset flags
    // moving_left = false;
    // moving_right = false;
    // moving_up = false;
    // moving_down = false;
}

void player_controller_post_frame(){
    // /*
    //     Apply velocity based on the flags we set this frame
    // */
    // player_ent->physics->velocity.x = 0;
    // player_ent->physics->velocity.y = 0;
    
    // if(moving_up)
    //     player_ent->physics->velocity.y = -10;
    // if(moving_down)
    //     player_ent->physics->velocity.y = 10;
    // if(moving_up && moving_down)
    //     player_ent->physics->velocity.y = 0;
    
    // if(moving_left)
    //     player_ent->physics->velocity.x = -10;
    // if(moving_right)
    //     player_ent->physics->velocity.x = 10;
    // if(moving_left && moving_right)
    //     player_ent->physics->velocity.x = 0;    
}