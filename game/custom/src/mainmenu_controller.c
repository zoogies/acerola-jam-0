/*
    Parts of this are yoinked from my last game:
    https://github.com/zoogies/raise-a-vannie
*/

#include <string.h>

#include <yoyoengine/yoyoengine.h>

#include "yoyo_c_api.h"

/*
    Since we have no save data, on game init we have predetermined state
*/
bool fullscreen = false;
int volume = 128;

struct ye_entity *quit_button = NULL;
struct ye_entity *play_button = NULL;
struct ye_entity *volume_up_button = NULL;
struct ye_entity *volume_down_button = NULL;
struct ye_entity *fullscreen_button = NULL;
struct ye_entity *volume_text = NULL;

void increase_volume() {
    if(volume < 128){
        volume += 8;
    }
    ye_set_volume((float)volume / 128);

    char volume_str[16];
    int volume_int = (int)(((float)volume) * 100 / 128);
    sprintf(volume_str, "%d%%", volume_int);

    free(volume_text->renderer->renderer_impl.text->text);
    volume_text->renderer->renderer_impl.text->text = strdup(volume_str);

    ye_update_renderer_component(volume_text);
}

void decrease_volume() {
    if(volume > 0){
        volume -= 8;
    }
    ye_set_volume((float)volume / 128);

    char volume_str[16];
    int volume_int = (int)(((float)volume) * 100 / 128);
    sprintf(volume_str, "%d%%", volume_int);

    free(volume_text->renderer->renderer_impl.text->text);
    volume_text->renderer->renderer_impl.text->text = strdup(volume_str);

    ye_update_renderer_component(volume_text);
}

void toggle_fullscreen() {
    if(fullscreen){
        ye_set_window_mode(0);

        free(fullscreen_button->renderer->renderer_impl.image->src);
        fullscreen_button->renderer->renderer_impl.image->src = strdup("images/ui/buttons/unticked.png");

        ye_update_renderer_component(fullscreen_button);
    }
    else{
        ye_set_window_mode(SDL_WINDOW_FULLSCREEN_DESKTOP);

        free(fullscreen_button->renderer->renderer_impl.image->src);
        fullscreen_button->renderer->renderer_impl.image->src = strdup("images/ui/buttons/ticked.png");
        
        ye_update_renderer_component(fullscreen_button);
    }
    fullscreen = !fullscreen;
}

void mainmenu_controller_poll(){
    if(quit_button != NULL){
        if(ye_button_hovered(quit_button)){
            quit_button->renderer->rotation = 7;
        } else {
            quit_button->renderer->rotation = 0;
        }
    }

    if(play_button != NULL){
        if(ye_button_hovered(play_button)){
            play_button->renderer->rotation = 7;

        } else {
            play_button->renderer->rotation = 0;
        }
    }

    if(volume_up_button != NULL){
        if(ye_button_hovered(volume_up_button)){
            volume_up_button->renderer->rotation = 7;

        } else {
            volume_up_button->renderer->rotation = 0;
        }
    }

    if(volume_down_button != NULL){
        if(ye_button_hovered(volume_down_button)){
            volume_down_button->renderer->rotation = 7;

        } else {
            volume_down_button->renderer->rotation = 0;
        }
    }

    if(fullscreen_button != NULL){
        if(ye_button_hovered(fullscreen_button)){
            fullscreen_button->renderer->rotation = 7;

        } else {
            fullscreen_button->renderer->rotation = 0;
        }
    }

    if(ye_button_clicked(quit_button)){
        ye_play_sound("sfx/pencil.mp3",0,1.0f);
        YG_RUNNING = false;
    }
    if(ye_button_clicked(play_button)){
        ye_play_sound("sfx/pencil.mp3",0,1.0f);

        // ye_load_scene_deferred("scenes/intro_cutscene.yoyo");
        ye_load_scene_deferred("scenes/lab.yoyo");
    }
    if(ye_button_clicked(volume_up_button)){
        ye_play_sound("sfx/pencil.mp3",0,1.0f);

        increase_volume();
    }
    if(ye_button_clicked(volume_down_button)){
        ye_play_sound("sfx/pencil.mp3",0,1.0f);

        decrease_volume();
    }
    if(ye_button_clicked(fullscreen_button)){
        ye_play_sound("sfx/pencil.mp3",0,1.0f);

        toggle_fullscreen();
    }
}

void init_mainmenu_controller() {
    quit_button = ye_get_entity_by_name("quit");
    play_button = ye_get_entity_by_name("play");
    volume_up_button = ye_get_entity_by_name("volume plus");
    volume_down_button = ye_get_entity_by_name("volume minus");
    fullscreen_button = ye_get_entity_by_name("fullscreen tick");
    volume_text = ye_get_entity_by_name("volume text");
}