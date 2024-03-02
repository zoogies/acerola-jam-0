
#include <yoyoengine/yoyoengine.h>
#include <math.h>
// hack to disable error (at compile time this is defined by libm):
#ifndef M_PI
#define M_PI 3.14
#endif

#include "player_controller.h"
#include "ui_controller.h"

bool player_moved_this_frame;

int mx = 0;
int my = 0;
int px = 0;
int py = 0;

struct ye_entity * player_ent = NULL;

struct bind_state_data sd;

void state_ui(struct nk_context *ctx){
    // struct nk_context * ctx = YE_STATE.engine.ctx;

    char mlb[20]; sprintf(mlb, "moving left:%d",sd.moving_left);
    char mrb[20]; sprintf(mrb, "moving right:%d",sd.moving_right);
    char mub[20]; sprintf(mub, "moving up:%d",sd.moving_up);
    char mdb[20]; sprintf(mdb, "moving down:%d",sd.moving_down);

    if (nk_begin(ctx, "STATE MACHINE", nk_rect(10, 10, 220, 200),
                    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label(ctx, mlb, NK_TEXT_LEFT);
        nk_label(ctx, mrb, NK_TEXT_LEFT);
        nk_label(ctx, mub, NK_TEXT_LEFT);
        nk_label(ctx, mdb, NK_TEXT_LEFT);
    }
    nk_end(ctx);
}

void init_player_controller(){
    player_ent = ye_get_entity_by_name("PLAYER");

    sd.bind_left = SDLK_a;
    sd.bind_right = SDLK_d;
    sd.bind_up = SDLK_w;
    sd.bind_down = SDLK_s;

    ui_controller_attatch();

    ui_refresh_bind_labels(sd);

    ui_register_component("player state machine",state_ui);

    // DEBUG SETTINGS
    YE_STATE.editor.colliders_visible = true;
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
    Correct state machine when we press or release a bind
    TODO: when binds change in the future we need to reset state machine to default
*/
void player_bind_abstraction(SDL_Event e){
    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == sd.bind_left){
            sd.moving_left = true;
            sd.discovered_left_bind = true;
            sd.discovered_bind_this_frame = true;
        }
        if(e.key.keysym.sym == sd.bind_right){
            sd.moving_right = true;
            sd.discovered_right_bind = true;
            sd.discovered_bind_this_frame = true;
        }
        if(e.key.keysym.sym == sd.bind_up){
            sd.moving_up = true;
            sd.discovered_up_bind = true;
            sd.discovered_bind_this_frame = true;
        }
        if(e.key.keysym.sym == sd.bind_down){
            sd.moving_down = true;
            sd.discovered_down_bind = true;
            sd.discovered_bind_this_frame = true;
        }
    }
    else if(e.type == SDL_KEYUP){
        if(e.key.keysym.sym == sd.bind_left){
            sd.moving_left = false;
        }
        if(e.key.keysym.sym == sd.bind_right){
            sd.moving_right = false;
        }
        if(e.key.keysym.sym == sd.bind_up){
            sd.moving_up = false;
        }
        if(e.key.keysym.sym == sd.bind_down){
            sd.moving_down = false;
        }
    }
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
    /*
        Schedule the velocity for this frame based on state machine
    */
    if(sd.moving_up && sd.moving_down)
        player_ent->physics->velocity.y = 0.0f;
    else if(sd.moving_up)
        player_ent->physics->velocity.y = -300.0f;
    else if(sd.moving_down)
        player_ent->physics->velocity.y = 300.0f;

    if(sd.moving_left && sd.moving_right)
        player_ent->physics->velocity.x = 0.0f;
    else if(sd.moving_left)
        player_ent->physics->velocity.x = -300.0f;
    else if(sd.moving_right)
        player_ent->physics->velocity.x = 300.0f;

    struct ye_rectf pos = ye_get_position(player_ent, YE_COMPONENT_TRANSFORM);
    // printf("%f,%f\n",pos.x,pos.y);
    ui_controller_update_bind_ui(sd, pos.x, pos.y);

    // WORKING NON PHYSICS SYSTEM IMPL
    // if(moving_up && moving_down){}
    // else if(moving_up)
    //     player_ent->transform->y -= 300.0f * YE_STATE.runtime.delta_time;
    // else if(moving_down)
    //     player_ent->transform->y += 300.0f * YE_STATE.runtime.delta_time;

    // if(moving_left && moving_right){}
    // else if(moving_left)
    //     player_ent->transform->x -= 300.0f * YE_STATE.runtime.delta_time;
    // else if(moving_right)
    //     player_ent->transform->x += 300.0f * YE_STATE.runtime.delta_time;
}

void player_controller_post_frame(){
    /*
        Reset the vel for next frame
    */
    player_ent->physics->velocity.x = 0.0f;
    player_ent->physics->velocity.y = 0.0f;

    // char * test = keycode_to_label(SDLK_w); 
    // if(test != NULL)
    //     printf("%s\n",test);
    // else
    //     printf("fail\n");
    // if(test != NULL)
    //     free(test);
}

void player_controller_trigger_handler(struct ye_entity * e1, struct ye_entity * e2){
    // randomize our abilities if we hit a randomizer, and remove that trigger
    if(strcmp(e1->name,"PLAYER") == 0 && strcmp(e2->name,"RANDOM_BIND_TRIGGER") == 0){
        sd.bind_up = SDLK_s;
        sd.bind_down = SDLK_w;
        sd.bind_right = SDLK_a;
        sd.bind_left = SDLK_d;
        sd.discovered_up_bind = false;
        sd.discovered_down_bind = false;
        sd.discovered_left_bind = false;
        sd.discovered_right_bind = false;
        ui_refresh_bind_labels(sd);
        ye_destroy_entity(e2);
    }
}

/*
    SHAPING:

    - binds are hidden until you hit them
        - REQUIRES a state to only update screen binds when needed or hit first time
*/