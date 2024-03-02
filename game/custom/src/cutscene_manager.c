/*
    yoinked from raise-a-vannie (my own game btw)
*/

#include <yoyoengine/yoyoengine.h>

#include "yoyo_c_api.h"

json_t *events = NULL;
int current_event = 0;

// redundant due to builtin sfx in slides
// const char sounds[4][20] = {"sfx/slide1.mp3","sfx/slide2.mp3","sfx/slide3.mp3","sfx/slide4.mp3"};
// int slide_itr = 0;

struct ye_entity *background = NULL;
struct ye_entity *next_button = NULL;

struct ye_entity *textbox = NULL;
struct ye_entity *dialog = NULL;
struct ye_entity *speaker = NULL;

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

    json_t *event = json_array_get(json_object_get(events, "events"), current_event);
    
    // once we reach the end, load the game
    if(event == NULL){
        ye_load_scene_deferred("scenes/lab.yoyo");
        return;
    }

    // set background src to the event "bg" key if exists
    json_t *bg = json_object_get(event, "bg");
    if(bg != NULL){
        free(background->renderer->renderer_impl.image->src);
        background->renderer->renderer_impl.image->src = strdup(json_string_value(bg));
        ye_update_renderer_component(background);
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
        speaker->active = true;
        free(dialog->renderer->renderer_impl.text->text);
        dialog->renderer->renderer_impl.text->text = strdup(json_string_value(text));
        ye_update_renderer_component(dialog);

        json_t *speaker_name = json_object_get(event, "speaker");
        if(speaker_name != NULL){
            free(speaker->renderer->renderer_impl.text->text);
            speaker->renderer->renderer_impl.text->text = strdup(json_string_value(speaker_name));
            ye_update_renderer_component(speaker);
        }
    }
    else{
        textbox->active = false;
        dialog->active = false;
        speaker->active = false;
    }

    // same with speaker
}

bool was_hovering_next = false;

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
    speaker = ye_get_entity_by_name("speaker name");

    setup_event();
}

void shutdown_cutscene(){
    if(events != NULL){
        json_decref(events);
    }
}