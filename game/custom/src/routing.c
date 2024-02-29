
#include <stdio.h>

#include <yoyoengine/yoyoengine.h>

#include "yoyo_c_api.h"
#include "player_controller.h"

enum scenes {
    MAINMENU,
    LAB
};

enum scenes current_scene;

/*
    On scene load, check what scene we are on and init or destruct things as needed
*/
void yoyo_scene_load(char *scene_name){
    if(strcmp(scene_name, "lab") == 0){
        if(current_scene != LAB){
            init_player_controller();
        }
        current_scene = LAB;
    }
    else{
        if(current_scene == LAB){
            shutdown_player_controller();
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
            player_controller_additional_render();
    }
}

void yoyo_pre_frame(){
    switch(current_scene){
        case LAB:
            player_controller_pre_frame();
    }
}

void yoyo_post_frame(){
    switch(current_scene){
        case LAB:
            player_controller_post_frame();
    }
}