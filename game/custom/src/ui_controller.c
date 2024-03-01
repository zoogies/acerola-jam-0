#include "player_controller.h"
#include "ui_controller.h"

#include <yoyoengine/yoyoengine.h>

struct ye_entity * up_key = NULL;
struct ye_entity * down_key = NULL;
struct ye_entity * left_key = NULL;
struct ye_entity * right_key = NULL;

struct ye_entity * up_text = NULL;
struct ye_entity * down_text = NULL;
struct ye_entity * left_text = NULL;
struct ye_entity * right_text = NULL;

struct ye_entity * vignette = NULL;

void ui_controller_attatch(){
    up_key = ye_get_entity_by_name("w_key");
    down_key = ye_get_entity_by_name("s_key");
    left_key = ye_get_entity_by_name("a_key");
    right_key = ye_get_entity_by_name("d_key");

    up_text = ye_get_entity_by_name("up_text");
    down_text = ye_get_entity_by_name("down_text");
    left_text = ye_get_entity_by_name("left_text");
    right_text = ye_get_entity_by_name("right_text");
    
    vignette = ye_get_entity_by_name("vignette");
}

/*
    note:
    for a really stupid reason (the animation being referenced at the root of one atlas image, not allowing us to conditionally tint per instance of its usage)
    we need a seperate animation meta and source to be in the cache for each key (this could be patched in engine with a renderer tint field but i dont want
    to spend more time than I need to when I can just patch it later)
*/
void ui_controller_update_bind_ui(struct bind_state_data sd, float x, float y){
    /*
        Move up
    */
    if(sd.moving_up){
        SDL_SetTextureColorMod(up_key->renderer->texture, 255, 0, 0);
    }
    else{
        SDL_SetTextureColorMod(up_key->renderer->texture, 255, 255, 255);
    }

    /*
        Move down
    */
    if(sd.moving_down){
        SDL_SetTextureColorMod(down_key->renderer->texture, 255, 0, 0);
    }
    else{
        SDL_SetTextureColorMod(down_key->renderer->texture, 255, 255, 255);
    }

    /*
        Move left
    */
    if(sd.moving_left){
        SDL_SetTextureColorMod(left_key->renderer->texture, 255, 0, 0);
    }
    else{
        SDL_SetTextureColorMod(left_key->renderer->texture, 255, 255, 255);
    }

    /*
        Move right
    */
    if(sd.moving_right){
        SDL_SetTextureColorMod(right_key->renderer->texture, 255, 0, 0);
    }
    else{
        SDL_SetTextureColorMod(right_key->renderer->texture, 255, 255, 255);
    }

    /*
        Update the transform of each bind ui item to be based on player
    */
    up_key->transform->x = x;
    up_key->transform->y = y;
    down_key->transform->x = x;
    down_key->transform->y = y;
    left_key->transform->x = x;
    left_key->transform->y = y;
    right_key->transform->x = x;
    right_key->transform->y = y;

    up_text->transform->x = x;
    up_text->transform->y = y;
    down_text->transform->x = x;
    down_text->transform->y = y;
    left_text->transform->x = x;
    left_text->transform->y = y;
    right_text->transform->x = x;
    right_text->transform->y = y;

    vignette->transform->x = x;
    vignette->transform->y = y;

    // ye_update_renderer_component(up_key);
    // ye_update_renderer_component(down_key);
    // ye_update_renderer_component(left_key);
    // ye_update_renderer_component(right_key);
}
