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
    Takes in a SDL keycode and returns a label for it
    (already malloced) - NULL for failure or unsopported keycode
*/
char * keycode_to_label(SDL_Keycode code){
    // TODO - imma shit my pants

    /*
        Malloc and assign it in this fn, then return pointer to, can swap the renderer pointer to it
    */

    char * label = (char*)malloc(2 * sizeof(char));
    label[1] = '\0';

    // TODO: maybe do this later idk
    // #define ktl(label, lower, upper) \
    //     case SDLK_##lower: label[0] = #upper; break;

    switch(code){
        case SDLK_a: label[0] = 'A'; break;
        case SDLK_b: label[0] = 'B'; break;
        case SDLK_c: label[0] = 'C'; break;
        case SDLK_d: label[0] = 'D'; break;
        case SDLK_e: label[0] = 'E'; break;
        case SDLK_f: label[0] = 'F'; break;
        case SDLK_g: label[0] = 'G'; break;
        case SDLK_h: label[0] = 'H'; break;
        case SDLK_i: label[0] = 'I'; break;
        case SDLK_j: label[0] = 'J'; break;
        case SDLK_k: label[0] = 'K'; break;
        case SDLK_l: label[0] = 'L'; break;
        case SDLK_m: label[0] = 'M'; break;
        case SDLK_n: label[0] = 'N'; break;
        case SDLK_o: label[0] = 'O'; break;
        case SDLK_p: label[0] = 'P'; break;
        case SDLK_q: label[0] = 'Q'; break;
        case SDLK_r: label[0] = 'R'; break;
        case SDLK_s: label[0] = 'S'; break;
        case SDLK_t: label[0] = 'T'; break;
        case SDLK_u: label[0] = 'U'; break;
        case SDLK_v: label[0] = 'V'; break;
        case SDLK_w: label[0] = 'W'; break;
        case SDLK_x: label[0] = 'X'; break;
        case SDLK_y: label[0] = 'Y'; break;
        case SDLK_z: label[0] = 'Z'; break;
        default:
            free(label);
            label = NULL;
    }

    return label;
}

/*
    Update render components for each label based on get_label_from_sdlk
*/
void ui_refresh_bind_labels(struct bind_state_data sd){
    // up
    free(up_text->renderer->renderer_impl.text->text);
    if(sd.discovered_up_bind)
        up_text->renderer->renderer_impl.text->text = keycode_to_label(sd.bind_up);
    else
        up_text->renderer->renderer_impl.text->text = strdup("?");
    ye_update_renderer_component(up_text);

    // down
    free(down_text->renderer->renderer_impl.text->text);
    if(sd.discovered_down_bind)
        down_text->renderer->renderer_impl.text->text = keycode_to_label(sd.bind_down);
    else
        down_text->renderer->renderer_impl.text->text = strdup("?");
    ye_update_renderer_component(down_text);

    // left
    free(left_text->renderer->renderer_impl.text->text);
    if(sd.discovered_left_bind)
    left_text->renderer->renderer_impl.text->text = keycode_to_label(sd.bind_left);
    else
        left_text->renderer->renderer_impl.text->text = strdup("?");
    ye_update_renderer_component(left_text);

    // right
    free(right_text->renderer->renderer_impl.text->text);
    if(sd.discovered_right_bind)
        right_text->renderer->renderer_impl.text->text = keycode_to_label(sd.bind_right);
    else
        right_text->renderer->renderer_impl.text->text = strdup("?");
    ye_update_renderer_component(right_text);
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
        If we discoevered a bind this frame, refresh the bind labels
    */
    if(sd.discovered_bind_this_frame){
        ui_refresh_bind_labels(sd);
        sd.discovered_bind_this_frame = false;
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
}
