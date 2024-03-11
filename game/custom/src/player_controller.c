
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include <yoyoengine/yoyoengine.h>

// hack to disable error (at compile time this is defined by libm):
#ifndef M_PI
#define M_PI 3.14
#endif

#include "player_controller.h"
#include "enemy_controller.h"
#include "ui_controller.h"
#include "zone.h"
#include "helper.h"

bool player_moved_this_frame;

int mx = 0;
int my = 0;
int px = 0;
int py = 0;

float player_look_angle = 0.0f;

struct ye_entity * player_ent = NULL;
struct ye_entity * arm_ent = NULL;

struct bind_state_data sd;

bool player_controller_active = false;

/*
    State related to arm animation attack
*/
bool    arm_attacking               = false;
int     arm_attack_duration         = 500;
int     arm_attack_cooldown         = 500;
int     arm_attack_poll_interval    = 5;
int     arm_attack_start            = 0;
float   arm_attack_relative_start   = 75.0f;

/*
    Footstep controller vars
*/
const char step_sounds[6][50] = {
    "sfx/footstep/concrete/1.mp3",
    "sfx/footstep/concrete/2.mp3",
    "sfx/footstep/concrete/3.mp3",
    "sfx/footstep/concrete/4.mp3",
    "sfx/footstep/concrete/5.mp3",
    "sfx/footstep/concrete/6.mp3"
};

bool footstep_cooldown = false;

/*
    Progression
*/
bool has_blue_card = false;
bool has_green_card = false;
bool has_keys = false;

void state_ui(struct nk_context *ctx){
    // struct nk_context * ctx = YE_STATE.engine.ctx;

    char mlb[20]; sprintf(mlb, "moving left:%d",sd.moving_left);
    char mrb[20]; sprintf(mrb, "moving right:%d",sd.moving_right);
    char mub[20]; sprintf(mub, "moving up:%d",sd.moving_up);
    char mdb[20]; sprintf(mdb, "moving down:%d",sd.moving_down);
    char att[20]; sprintf(att, "attacking:%d",sd.attacking);

    if (nk_begin(ctx, "STATE MACHINE", nk_rect(10, 10, 220, 200),
                    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label(ctx, mlb, NK_TEXT_LEFT);
        nk_label(ctx, mrb, NK_TEXT_LEFT);
        nk_label(ctx, mub, NK_TEXT_LEFT);
        nk_label(ctx, mdb, NK_TEXT_LEFT);
        nk_label(ctx, att, NK_TEXT_LEFT);
    }
    nk_end(ctx);
}

void init_player_controller(){
    player_ent = ye_get_entity_by_name("PLAYER");
    arm_ent = ye_get_entity_by_name("ARM");
    // set the center of rotation (engine does not serialize this)
    // arm_ent->renderer->center = (SDL_Point){0, arm_ent->renderer->rect.h}; // bottom left corner

    sd.bind_left = SDLK_a;
    sd.bind_right = SDLK_d;
    sd.bind_up = SDLK_w;
    sd.bind_down = SDLK_s;
    sd.bind_attack = SDLK_f;

    ui_controller_attatch();

    ui_refresh_bind_labels(sd);

    // ui_register_component("player state machine",state_ui);

    // DEBUG SETTINGS
    // YE_STATE.editor.colliders_visible = true;

    // ui_register_component("timer view", ye_timer_overlay);

    char * sample[] = {"what the hell?","where am I?"};
    begin_dialog(sample,2, NULL);
}

void shutdown_player_controller(){
    player_ent = NULL;
    arm_ent = NULL;
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
    if(!player_controller_active){
        sd.moving_down = false;
        sd.moving_left = false;
        sd.moving_up = false;
        sd.moving_down = false;
        return;
    }

    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == sd.bind_left){
            sd.moving_left = true;
            if(!sd.discovered_left_bind){
                sd.discovered_left_bind = true;
                sd.discovered_bind_this_frame = true;
            }
        }
        if(e.key.keysym.sym == sd.bind_right){
            sd.moving_right = true;
            if(!sd.discovered_right_bind){
                sd.discovered_right_bind = true;
                sd.discovered_bind_this_frame = true;
            }
        }
        if(e.key.keysym.sym == sd.bind_up){
            sd.moving_up = true;
            if(!sd.discovered_up_bind){
                sd.discovered_up_bind = true;
                sd.discovered_bind_this_frame = true;
            }
        }
        if(e.key.keysym.sym == sd.bind_down){
            sd.moving_down = true;
            if(!sd.discovered_down_bind){
                sd.discovered_down_bind = true;
                sd.discovered_bind_this_frame = true;
            }
        }
        if(e.key.keysym.sym == sd.bind_attack){
            // printf("pressed attack bind\n");
            // arm_attacking = true;
            sd.attacking = true;
            if(!arm_attacking){
                // printf("!arm_attacking, dispatching begin call\n");
                begin_arm_attack_anim();
            }
            if(!sd.discovered_attack_bind){
                sd.discovered_attack_bind = true;
                sd.discovered_bind_this_frame = true;
            }
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
        if(e.key.keysym.sym == sd.bind_attack){
            sd.attacking = false;
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

    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_SPACE){
            ui_progress_dialog();
        }
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
    player_look_angle = (float)angle;
    // player_ent->renderer->rotation = (float)angle;

    /*
        Handle movement
    */
    player_bind_abstraction(e);

    // TODO: optimization, do this once a frame instead of once an event (can be many in a frame)
}

void cancel_footstep_cooldown(){
    footstep_cooldown = false;
}


/*
    Here we will route through custom input handler and then into the player controller,
    use some kind of state machine to define up down left right and actions, then we just have an 
    input layer abstracting those to the changing keybinds
*/

void player_controller_pre_frame(){
    if(player_controller_active){
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
    }
    else{
        sd.moving_down = false;
        sd.moving_left = false;
        sd.moving_up = false;
        sd.moving_down = false;
    }

    /*
        If we are moving at all, lets play a sound if we arent on cooldown
    */
    if(
        (sd.moving_down || sd.moving_up || sd.moving_left || sd.moving_right)
        && !footstep_cooldown
    ){
        footstep_cooldown = true;

        // register timer to reset cooldown in 1 second
        struct ye_timer * ft = malloc(sizeof(struct ye_timer));
        ft->callback = cancel_footstep_cooldown;
        ft->start_ticks = -1;
        ft->length_ms = 500;
        ft->loops = 0;
        ye_register_timer(ft);

        // pick a footstep sound and play it
        int roll = rand() % sizeof(step_sounds) / sizeof(step_sounds[0]);
        // printf("rolled sound %d\n",roll);

        ye_play_sound(step_sounds[roll],0,0.5f);
    }

    struct ye_rectf pos = ye_get_position(player_ent, YE_COMPONENT_TRANSFORM);
    // printf("%f,%f\n",pos.x,pos.y);
    ui_controller_update_bind_ui(&sd, pos.x, pos.y);

    // make sure the arm is tracked to the player
    arm_ent->transform->x = player_ent->transform->x;
    arm_ent->transform->y = player_ent->transform->y;

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

    /*
        Set the look angle
    */
    player_ent->renderer->rotation = player_look_angle;

    // char * test = keycode_to_label(SDLK_w); 
    // if(test != NULL)
    //     printf("%s\n",test);
    // else
    //     printf("fail\n");
    // if(test != NULL)
    //     free(test);
}

void player_transform(SDL_Keycode new_bind_up, SDL_Keycode new_bind_down, SDL_Keycode new_bind_left, SDL_Keycode new_bind_right, SDL_Keycode new_bind_attack){
    // play transform sound
    ye_play_sound("sfx/transform.mp3", 0, 1.0f);
    
    splat_blood(player_ent->transform->x, player_ent->transform->y);
    splat_blood(player_ent->transform->x, player_ent->transform->y);
    
    begin_ui_transform_blood();
    
    sd.bind_up = new_bind_up;
    sd.bind_down = new_bind_down;
    sd.bind_right = new_bind_left;
    sd.bind_left = new_bind_right;
    sd.bind_attack = new_bind_attack;
    sd.discovered_up_bind = false;
    sd.discovered_down_bind = false;
    sd.discovered_left_bind = false;
    sd.discovered_right_bind = false;
    sd.discovered_attack_bind = false;
    sd.moving_up = false;
    sd.moving_down = false;
    sd.moving_left = false;
    sd.moving_right = false;
    sd.attacking = false;
    ui_refresh_bind_labels(sd);
}

void post_transform_deadend_dialog(){
    char * sample[] = {"What the fuck?!?!?","I feel like im losing control"};
    
    sd.moving_up = false;
    sd.moving_down = false;
    sd.moving_left = false;
    sd.moving_right = false;
    sd.attacking = false;
    
    begin_dialog(sample,2, NULL);
}

void transform_post_deadend(){
    player_transform(SDLK_s, SDLK_w, SDLK_a, SDLK_d, SDLK_e);

    // schedule poll timer
    struct ye_timer * t = malloc(sizeof(struct ye_timer));
    t->callback = post_transform_deadend_dialog;
    t->length_ms = 1500;
    t->loops = 0;
    t->start_ticks = SDL_GetTicks();
    ye_register_timer(t);
}

bool player_seen_blue_door_unopened = false;
bool player_seen_green_door_unopened = false;
bool player_seen_locked_front_door = false;

void blue_access_only(){
    char * sample[] = {"BLUE ACCESS REQUIRED. AUTHORIZED PERSONAL ONLY."};
    set_fonts_and_colors_dialog("typewriter","white",30);
    begin_dialog(sample,1, reset_fonts_and_colors_dialog);
}

void green_access_only(){
    char * sample[] = {"Dr. Alverick's Office (nuerosurgery)"};
    set_fonts_and_colors_dialog("typewriter","white",30);
    begin_dialog(sample,1, reset_fonts_and_colors_dialog);
}

void player_controller_trigger_handler(struct ye_entity * e1, struct ye_entity * e2){
    // randomize our abilities if we hit a randomizer, and remove that trigger
    if(strcmp(e1->name,"PLAYER") == 0 && strcmp(e2->name,"RANDOM_BIND_TRIGGER") == 0){
        player_transform(SDLK_s, SDLK_w, SDLK_a, SDLK_d, SDLK_e);
        ye_destroy_entity(e2);
    }

    if(strcmp(e1->name,"PLAYER") == 0 && strcmp(e2->name,"blue door floor") == 0){
        if(has_blue_card){
            // sd.moving_up = false;
            // sd.moving_down = false;
            // sd.moving_left = false;
            // sd.moving_right = false;
            // sd.attacking = false;
            ye_destroy_entity(e2);

            ye_play_sound("sfx/keyswipe.wav",0,1.0f);
            unlock_by_tag("black_zone_2");

            struct ye_entity *door = ye_get_entity_by_name("METAL_DOOR_PROGRESSION_1");
            door->renderer->rotation = -90;
            door->renderer->rect.x = -145;
            door->renderer->rect.y = -110;
            door->collider->rect = (struct ye_rectf){-51,-175,95,285};

            player_transform(SDLK_s, SDLK_a, SDLK_q, SDLK_z, SDLK_d);
            create_new_enemy(186,-2232);
        }
        else{
            if(!player_seen_blue_door_unopened){
                sd.moving_up = false;
                sd.moving_down = false;
                sd.moving_left = false;
                sd.moving_right = false;
                sd.attacking = false;
                char * sample[] = {"There is a keycard reader on the door frame.","A piece of paper attatched to the door reads"};
                begin_dialog(sample,2, blue_access_only);
            }
            player_seen_blue_door_unopened = true;
        }
    }

    if(strcmp(e1->name,"PLAYER") == 0 && strcmp(e2->name,"green floor") == 0){
        if(has_green_card){
            // sd.moving_up = false;
            // sd.moving_down = false;
            // sd.moving_left = false;
            // sd.moving_right = false;
            // sd.attacking = false;
            ye_destroy_entity(e2);

            ye_play_sound("sfx/keyswipe.wav",0,1.0f);
            unlock_by_tag("green_room");

            struct ye_entity *door = ye_get_entity_by_name("GREEN ROOM DOOR");
            door->renderer->rotation = 90;
            door->transform->x = 2049;
            door->transform->x = -2285;
            ye_remove_collider_component(door);
        }
        else{
            if(!player_seen_green_door_unopened){
                sd.moving_up = false;
                sd.moving_down = false;
                sd.moving_left = false;
                sd.moving_right = false;
                sd.attacking = false;
                char * sample[] = {"There is a keycard reader on the door frame.","The nameplate next to the door reads"};
                begin_dialog(sample,2, green_access_only);
            }
            player_seen_green_door_unopened = true;
        }
    }

    if(strcmp(e1->name,"PLAYER") == 0 && strcmp(e2->name,"clipboard_1") == 0){
        ye_remove_collider_component(e2);
        char * sample[] = {"Operation Report: 6O% blood loss, patient recovering well.","Expected awakening: 22OO hours."};
        set_fonts_and_colors_dialog("typewriter","white",30);
        
        sd.moving_up = false;
        sd.moving_down = false;
        sd.moving_left = false;
        sd.moving_right = false;
        sd.attacking = false;
        
        begin_dialog(sample,2, reset_fonts_and_colors_dialog);
    }

    if(strcmp(e1->name,"PLAYER") == 0 && strcmp(e2->name,"blue_card_pickup") == 0){
        ye_destroy_entity(e2);
        ye_play_sound("sfx/pickup.wav",0,0.5f);
        has_blue_card = true;
    }

    if(strcmp(e1->name,"PLAYER") == 0 && strcmp(e2->name,"green_card_pickup") == 0){
        ye_destroy_entity(e2);
        ye_play_sound("sfx/pickup.wav",0,0.5f);
        has_green_card = true;
    }

    if(strcmp(e1->name,"PLAYER") == 0 && strcmp(e2->name,"key_card_pickup") == 0){
        ye_destroy_entity(e2);
        ye_play_sound("sfx/pickup.wav",0,0.5f);
        has_keys = true;
    }

    if(strcmp(e1->name,"PLAYER") == 0 && strcmp(e2->name,"chair_trigger") == 0){
        sd.moving_up = false;
        sd.moving_down = false;
        sd.moving_left = false;
        sd.moving_right = false;
        sd.attacking = false;
        
        char * sample[] = {"I feel like... I could break this","I just need to figure out how..."};
        begin_dialog(sample,2, NULL);
        ye_destroy_entity(e2);
    }

    if(strcmp(e1->name,"PLAYER") == 0 && strcmp(e2->name,"frontdoor trigger") == 0){
        if(has_keys){
            // TODO: END GAME
        }
        else if(!player_seen_locked_front_door){
            sd.moving_up = false;
            sd.moving_down = false;
            sd.moving_left = false;
            sd.moving_right = false;
            sd.attacking = false;
            
            char * sample[] = {"This seems to be the exit!","A giant padlock hangs, chained through the handles."};
            begin_dialog(sample,2, NULL);
            // ye_destroy_entity(e2);
            player_seen_locked_front_door = true;
        }
    }

    if(strcmp(e1->name,"PLAYER") == 0 && strcmp(e2->name,"dead end trigger") == 0){
        sd.moving_up = false;
        sd.moving_down = false;
        sd.moving_left = false;
        sd.moving_right = false;
        sd.attacking = false;
        
        char * sample[] = {"Something feels wrong...","I think... Im going to be sick...","..................."};
        begin_dialog(sample,3,transform_post_deadend);
        ye_destroy_entity(e2);
    }
}

void arm_attack_cb_poll(){
    int current_time = SDL_GetTicks() - arm_attack_start;

    // check if animation should be over
    if(current_time > arm_attack_duration){
        arm_ent->active = false;
        // printf("we are over attack duration, hiding\n");
        
        if(current_time > arm_attack_duration + arm_attack_cooldown){
            // printf("cooldown is also over, cancelling reschedule\n");
            arm_attacking = false;
            return;
        }
    }
    else{
        // printf("calculating and updating swing\n");
        /*
            NOTE: we only do the math if we are visibly swinging
            We should swing 180 degrees from the start point, but synced to the players face turn
        */
        float progress_through_swing = (float)((float)current_time / (float)arm_attack_duration);
        // printf("swing progress: %f\n",progress_through_swing);

        // make it expoenential for some impact
        progress_through_swing = pow(progress_through_swing, 2);

        // sync the rotation
        float momentary_rotation = 180.0f * progress_through_swing;
        arm_ent->renderer->rotation = player_look_angle - momentary_rotation + arm_attack_relative_start;
    
        // printf("player: %f\nmomentary: %f\nrelative_offset: %f\n",player_look_angle,momentary_rotation,arm_attack_relative_start);
    }

    // re register timer
    struct ye_timer * t = malloc(sizeof(struct ye_timer));
    t->callback = arm_attack_cb_poll;
    t->length_ms = arm_attack_poll_interval; // poll every 10ms
    t->loops = 0;
    t->start_ticks = SDL_GetTicks();
    ye_register_timer(t);

    // printf("re scheduled timer\n");
}

bool room_one_blocked = true;
bool blue_room_blocked = true;
bool lobby_blocked = true;
bool green_card_hallway_blocked = true;
bool green_card_room_blocked = true;

void begin_arm_attack_anim(){
    // printf("was told to start anim\n");
    if(arm_attacking){
        // printf("already playing. not started\n");
        return;
    }

    // get the center of the player model to calculate from
    int cx = player_ent->transform->x + (player_ent->renderer->rect.w / 2);
    int cy = player_ent->transform->y + (player_ent->renderer->rect.h / 2);

    //////////////////////////////////////

    /*
        This is kinda stupid CQ wise, but im just going to
        check for the one or two relevant progression things inside this function
    */

    if(room_one_blocked){
        // break intro room blockage
        struct ye_entity *block = ye_get_entity_by_name("char_render");
        if(abs(ye_distance(
            cx,
            cy,
            block->transform->x + (block->renderer->rect.w / 2),
            block->transform->y + (block->renderer->rect.h / 2)
        )) < 200){
            // we are close enough to break door
            ye_play_sound("sfx/woodbreak.mp3",0,0.3f);
            ye_remove_collider_component(block);
            free(block->renderer->renderer_impl.image->src);
            block->renderer->renderer_impl.image->src = strdup("images/env/brokenboard.png");
            ye_update_renderer_component(block);
            unlock_by_tag("black_zone_1");
            room_one_blocked = false;
        }
    }

    if(blue_room_blocked){
        // break intro room blockage
        struct ye_entity *block = ye_get_entity_by_name("blueroomdoor");
        if(abs(ye_distance(
            cx,
            cy,
            block->transform->x + (block->renderer->rect.w / 2),
            block->transform->y + (block->renderer->rect.h / 2)
        )) < 200){
            // we are close enough to break door
            ye_play_sound("sfx/woodbreak.mp3",0,0.3f);
            ye_remove_collider_component(block);
            free(block->renderer->renderer_impl.image->src);
            block->renderer->renderer_impl.image->src = strdup("images/env/brokenboard.png");
            ye_update_renderer_component(block);
            unlock_by_tag("blue_room");
            blue_room_blocked = false;

            // spawn room enemies
            create_new_enemy(2605,-3412);
            create_new_enemy(3204,-3311);
            create_new_enemy(3698,-2938);
            create_new_enemy(2922,-2822);
        }
    }

    if(lobby_blocked){
        // break intro room blockage
        struct ye_entity *block = ye_get_entity_by_name("lobby block");
        if(abs(ye_distance(
            cx,
            cy,
            block->transform->x + (block->renderer->rect.w / 2),
            block->transform->y + (block->renderer->rect.h / 2)
        )) < 450){
            // we are close enough to break door
            ye_play_sound("sfx/woodbreak.mp3",0,0.3f);
            ye_remove_collider_component(block);
            free(block->renderer->renderer_impl.image->src);
            block->renderer->renderer_impl.image->src = strdup("images/env/brokenboard.png");
            ye_update_renderer_component(block);
            unlock_by_tag("lobby");
            lobby_blocked = false;

            // spawn room enemies
            create_new_enemy(-1012,-2090);
            create_new_enemy(-1036,-1797);
            create_new_enemy(-750,-1500);
            create_new_enemy(-1600,-1910);
        }
    }

    if(green_card_hallway_blocked){
        // break intro room blockage
        struct ye_entity *block = ye_get_entity_by_name("greencard hall door");
        if(abs(ye_distance(
            cx,
            cy,
            block->transform->x + (block->renderer->rect.w / 2),
            block->transform->y + (block->renderer->rect.h / 2)
        )) < 250){
            // we are close enough to break door
            ye_play_sound("sfx/woodbreak.mp3",0,0.3f);
            ye_remove_collider_component(block);
            free(block->renderer->renderer_impl.image->src);
            block->renderer->renderer_impl.image->src = strdup("images/env/brokenboard.png");
            ye_update_renderer_component(block);
            unlock_by_tag("green_hallway");
            green_card_hallway_blocked = false;

            // spawn room enemies
            // create_new_enemy(-1012,-2090);
            // create_new_enemy(-1036,-1797);
            // create_new_enemy(-750,-1500);
            // create_new_enemy(-1600,-1910);
        }
    }

    if(green_card_room_blocked){
        // break intro room blockage
        struct ye_entity *block = ye_get_entity_by_name("greencard hall door copy");
        if(abs(ye_distance(
            cx,
            cy,
            block->transform->x + (block->renderer->rect.w / 2),
            block->transform->y + (block->renderer->rect.h / 2)
        )) < 250){
            // we are close enough to break door
            ye_play_sound("sfx/woodbreak.mp3",0,0.3f);
            ye_remove_collider_component(block);
            free(block->renderer->renderer_impl.image->src);
            block->renderer->renderer_impl.image->src = strdup("images/env/brokenboard.png");
            ye_update_renderer_component(block);
            unlock_by_tag("green_card_room");
            green_card_room_blocked = false;

            // spawn room enemies
            // create_new_enemy(-1012,-2090);
            // create_new_enemy(-1036,-1797);
            // create_new_enemy(-750,-1500);
            // create_new_enemy(-1600,-1910);
        }
    }

    //////////////////////////////////////
    
    int r = rand() % 3;
    switch(r){
        case 1:
            ye_play_sound("sfx/armswing.mp3",0,0.5f);
            break;
        case 2:
            ye_play_sound("sfx/armswing2.mp3",0,0.5f);
            break;
        default:
            ye_play_sound("sfx/armswing3.mp3",0,0.5f);
            break;
    }
    arm_attacking = true;
    arm_attack_start = SDL_GetTicks();
    arm_ent->active = true;
    // sync start point to where player is looking
    arm_ent->renderer->rotation = player_look_angle + arm_attack_relative_start;

    // register the polling function
    struct ye_timer * t = malloc(sizeof(struct ye_timer));
    t->callback = arm_attack_cb_poll;
    t->length_ms = arm_attack_poll_interval; // poll every 10ms
    t->loops = 0;
    t->start_ticks = arm_attack_start;
    ye_register_timer(t);

    // try to kill enemies within range
    kill_enemies_within(cx,cy, player_look_angle);
}

/*
    SHAPING:

    - binds are hidden until you hit them
        - REQUIRES a state to only update screen binds when needed or hit first time
*/