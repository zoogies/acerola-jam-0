
#include <stdio.h>

#include <yoyoengine/yoyoengine.h>

#include "yoyo_c_api.h"
#include "player_controller.h"
#include "cutscene_manager.h"
#include "ui_controller.h"
#include "enemy_controller.h"
#include "zone.h"

enum scenes {
    MAINMENU,
    INTRO_CUTSCENE,
    LAB
};

enum scenes current_scene;

/*
    On scene load, check what scene we are on and init or destruct things as needed
*/
void yoyo_scene_load(char *scene_name){
    enum scenes last_scene = current_scene;

    if(strcmp(scene_name, "cutscene_base") == 0){
        if(current_scene != INTRO_CUTSCENE){
            init_cutscene("cutscenes/intro.json");
        }
        current_scene = INTRO_CUTSCENE;
    }
    else{
        if(last_scene == INTRO_CUTSCENE){
            shutdown_cutscene();
        }
    }
    
    if(strcmp(scene_name, "lab") == 0){
        if(current_scene != LAB){
            init_player_controller();
            enemy_controller_init();
            lock_zone_one();
        }
        current_scene = LAB;
    }
    else{
        if(last_scene == LAB){
            shutdown_player_controller();
            enemy_controller_shutdown();
        }
    }
}

void yoyo_handle_input(SDL_Event event){
    // we need to do our own poll for quit when we have our own input handler:
    if(event.type == SDL_QUIT){
        YG_RUNNING = false;
    }
    
    switch(current_scene){
        case LAB:
            player_input_handler(event);
    }
}

void yoyo_pre_shutdown(){
    switch(current_scene){
        case LAB:
            shutdown_player_controller();
    }
}

void yoyo_additional_render(){
    switch(current_scene){
        case LAB:
            ui_controller_additional_render();
            player_controller_additional_render();
    }
}

void yoyo_pre_frame(){
    switch(current_scene){
        case LAB:
            player_controller_pre_frame();
            enemy_controller_update();
        break;
    }
}

void yoyo_post_frame(){
    switch(current_scene){
        case INTRO_CUTSCENE:
            cutscene_poll();
            break;
        case LAB:
            player_controller_post_frame();
            break;
    }
}

void yoyo_trigger_enter(struct ye_entity *e1, struct ye_entity *e2){
    // printf("%s was entered by %s\n",e2->name, e1->name);

    player_controller_trigger_handler(e1,e2);
}

void yoyo_collision(struct ye_entity *e1, struct ye_entity *e2){
    // printf("%s collided with %s\n",e2->name, e1->name);
}