/*
    yoinked from raise-a-vannie (my own game btw)
*/

#include <math.h>

#include <yoyoengine/yoyoengine.h>

#include "yoyo_c_api.h"

json_t *events = NULL;
int current_event = 0;

// redundant due to builtin sfx in slides
const char sounds[4][20] = {"sfx/slide1.mp3","sfx/slide2.mp3","sfx/slide3.mp3","sfx/slide4.mp3"};
// int slide_itr = 0;

struct ye_entity *background = NULL;
struct ye_entity *next_button = NULL;

struct ye_entity *textbox = NULL;
struct ye_entity *dialog = NULL;
// struct ye_entity *speaker = NULL;

/*
    A decent amount of code copy pasted from ui_controller.c to
    allow character insertion through the text box like in game
*/

char *  _current_line           = NULL;
int     _char_index_into_str    = 0;

char _character_noises[3][25] = {"sfx/keypress/1.mp3","sfx/keypress/2.mp3","sfx/keypress/3.mp3"};
int _ms_per_character = 50;

/*
    ===========================================================
*/


void setup_event(){
    /*
        the events is strucutred like
        {
            "events":[
                {...}
            ]
        }

        get the event from the array index, if we are at the end load "scenes/game.yoyo"
    */

    // play a random slide sound effect
    int r = rand()%4;
    ye_play_sound(sounds[r],0,1.0f);

    json_t *event = json_array_get(json_object_get(events, "events"), current_event);
    
    // once we reach the end, load the game
    if(event == NULL){
        ye_load_scene_deferred("scenes/lab.yoyo");
        return;
    }

    // set background src to the event "bg" key if exists
    json_t *bg = json_object_get(event, "bg");
    if(bg != NULL){
        free(background->renderer->renderer_impl.animation->meta_file);
        background->renderer->renderer_impl.animation->meta_file = strdup(json_string_value(bg));
        ye_update_renderer_component(background);
        // update the size of the bg
        background->renderer->rect.w = 1920;
        background->renderer->rect.h = 1080;
        background->renderer->z = 0;
    }


    // if sfx exists, play it
    json_t *sfx = json_object_get(event, "sfx");
    if(sfx != NULL){
        ye_play_sound(json_string_value(sfx), 0, 1.0f);
    }

    // if text exists, set the textbox to active, else false
    json_t *text = json_object_get(event, "text");
    if(text != NULL){
        textbox->active = true;
        dialog->active = true;
        // speaker->active = true;
        // free(dialog->renderer->renderer_impl.text->text);
        // dialog->renderer->renderer_impl.text->text = strdup(json_string_value(text));
        // ye_update_renderer_component(dialog);

        // json_t *speaker_name = json_object_get(event, "speaker");
        // if(speaker_name != NULL){
            // free(speaker->renderer->renderer_impl.text->text);
            // speaker->renderer->renderer_impl.text->text = strdup(json_string_value(speaker_name));
            // ye_update_renderer_component(speaker);
        // }

        _current_line = json_string_value(text);
        _char_index_into_str = 0;
    }
    else{
        textbox->active = false;
        dialog->active = false;
        // speaker->active = false;
        _current_line = NULL;
    }

    // same with speaker
}

bool was_hovering_next = false;

int last_cutscene_text_tick = 0;

void cutscene_poll(){
    if(next_button != NULL){
        // if(ye_button_hovered(next_button)){
        //     free(next_button->renderer->renderer_impl.animation->meta_file);
        //     next_button->renderer->renderer_impl.animation->meta_file = strdup("animations/nextbuttonhover/nextbuttonhover.yoyo");
        //     ye_update_renderer_component(next_button);

        //     was_hovering_next = true;
        // }
        // else{
        //     if(was_hovering_next){
        //         free(next_button->renderer->renderer_impl.animation->meta_file);
        //         next_button->renderer->renderer_impl.animation->meta_file = strdup("animations/nextbutton/nextbutton.yoyo");
        //         ye_update_renderer_component(next_button);
        //     }
        //     was_hovering_next = false;
        // }

        /*
            Attempt to run functionality here instead of timer
        */
        if(SDL_GetTicks() - last_cutscene_text_tick > _ms_per_character && _current_line != NULL){
            if(_char_index_into_str < strlen(_current_line)){
                char substr[1024];
                strncpy(substr,_current_line,_char_index_into_str + 1);
                substr[_char_index_into_str + 1] = '\0';
            
                free(dialog->renderer->renderer_impl.text->text);
                dialog->renderer->renderer_impl.text->text = strdup(substr);
                ye_update_renderer_component(dialog);

                _char_index_into_str++;
                int r = rand() % 3;
                ye_play_sound(_character_noises[r],0,0.75f);
            }
            last_cutscene_text_tick = SDL_GetTicks();
        }

        if(ye_button_hovered(next_button)){
            next_button->renderer->rotation = -5;
        }
        else{
            next_button->renderer->rotation = 0;
        }

        if(ye_button_clicked(next_button)){
            current_event++;
            setup_event();
            // ye_play_sound("sfx/select.wav", 0, 0.75f); select slide noise baked into event json
        }
    }
}

void init_cutscene(const char * scene_path){
    current_event = 0;
    events = yep_resource_json(scene_path);
    if(events == NULL){
        ye_logf(error, "Failed to load intro cutscene events\n");
        YG_RUNNING = false;
    }

    background = ye_get_entity_by_name("cutscene bg");
    next_button = ye_get_entity_by_name("next button");
    textbox = ye_get_entity_by_name("textbox");
    dialog = ye_get_entity_by_name("dialog text");
    // speaker = ye_get_entity_by_name("speaker name");

    free(dialog->renderer->renderer_impl.text->text);
    dialog->renderer->renderer_impl.text->text = strdup(" ");
    ye_update_renderer_component(dialog);

    setup_event();

    // // schedule poll timer
    // struct ye_timer * t = malloc(sizeof(struct ye_timer));
    // t->callback = intro_poll_dialog;
    // t->length_ms = _ms_per_character;
    // t->loops = -1;
    // t->start_ticks = SDL_GetTicks();
    // ye_register_timer(t);
}

void shutdown_cutscene(){
    if(events != NULL){
        json_decref(events);
    }
}