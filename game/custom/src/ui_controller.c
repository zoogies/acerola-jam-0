#include <math.h>

#include "player_controller.h"
#include "ui_controller.h"

#include <yoyoengine/yoyoengine.h>

struct ye_entity * up_key = NULL;
struct ye_entity * down_key = NULL;
struct ye_entity * left_key = NULL;
struct ye_entity * right_key = NULL;
struct ye_entity * attack_key = NULL;

struct ye_entity * up_text = NULL;
struct ye_entity * down_text = NULL;
struct ye_entity * left_text = NULL;
struct ye_entity * right_text = NULL;
struct ye_entity * attack_text = NULL;

struct ye_entity * vignette = NULL;
struct ye_entity * bloody_vignette = NULL;
struct ye_entity * darken = NULL;

// monitor
struct ye_entity * monitor = NULL;
struct ye_entity * monitor_graph = NULL;
struct ye_entity * monitor_heartrate = NULL;

// text box stuff
struct ye_entity * text_box = NULL;
struct ye_entity * text_content = NULL;
struct ye_entity * text_continue_prompt = NULL;

/*
    Film grain
*/
SDL_Texture * tex_noise[20];

/*
    Heartrate monitor timer state
*/
int heartrate = 80; // try to keep heartrate 80-220
int hr_interval = 1250;
int hr_poll_interval = 50;
int last_hr_tick = 0;

void set_heartrate(int hr){
    heartrate = hr;

    hr_interval = 2500 - (heartrate * 10);

    // make sure that our animation is playing at the right rate
    int num_frames = monitor_graph->renderer->renderer_impl.animation->frame_count;

    int desired_delay = hr_interval / num_frames;
    monitor_graph->renderer->renderer_impl.animation->frame_delay = desired_delay;

    _randomize_heartrate_text();
}

/*
    Responsible for speeding up/slowing down HR sounds and display
*/
void heartrate_monitor_poll(){
    hr_interval = 2500 - (heartrate * 10);

    int current_ticks = SDL_GetTicks();
    if(current_ticks - last_hr_tick > hr_interval){
        last_hr_tick = current_ticks;

        int channel = ye_play_sound("sfx/monitor/beep.mp3", 0, 1.0f);
        Mix_SetPosition(channel, 135, 10);
        // printf("badump\n");

        // make sure that our animation is playing at the right rate
        int num_frames = monitor_graph->renderer->renderer_impl.animation->frame_count;

        int desired_delay = hr_interval / num_frames;
        monitor_graph->renderer->renderer_impl.animation->frame_delay = desired_delay;
    }

    // re schedule timer
    struct ye_timer * t = malloc(sizeof(struct ye_timer));
    t->callback = heartrate_monitor_poll;
    t->length_ms = hr_poll_interval;
    t->loops = 0;
    t->start_ticks = SDL_GetTicks();
    ye_register_timer(t);
}

void _randomize_heartrate_text(){
    free(monitor_heartrate->renderer->renderer_impl.text->text);
    char n[3];
    int offset = (rand() % 10) - 5;
    sprintf(n, "%d", heartrate + offset); 
    monitor_heartrate->renderer->renderer_impl.text->text = strdup(n);
    ye_update_renderer_component(monitor_heartrate);
}

void randomize_heartrate_text(){
    _randomize_heartrate_text();

    // re schedule to run
    struct ye_timer * t2 = malloc(sizeof(struct ye_timer));
    t2->callback = randomize_heartrate_text;
    
    if(heartrate > 180){
        t2->length_ms = (rand() % 300) + 300;
    }
    else if(heartrate > 100){
        t2->length_ms = (rand() % 600) + 600;
    }
    else{
        t2->length_ms = (rand() % 600) + 1000;
    }
    t2->loops = 0;
    t2->start_ticks = SDL_GetTicks();
    ye_register_timer(t2);
}

void ui_controller_attatch(){
    up_key = ye_get_entity_by_name("w_key");
    down_key = ye_get_entity_by_name("s_key");
    left_key = ye_get_entity_by_name("a_key");
    right_key = ye_get_entity_by_name("d_key");
    attack_key = ye_get_entity_by_name("attack_key");

    up_text = ye_get_entity_by_name("up_text");
    down_text = ye_get_entity_by_name("down_text");
    left_text = ye_get_entity_by_name("left_text");
    right_text = ye_get_entity_by_name("right_text");
    attack_text = ye_get_entity_by_name("attack_text");
    
    vignette = ye_get_entity_by_name("vignette");
    darken = ye_get_entity_by_name("dark overlay");
    bloody_vignette = ye_get_entity_by_name("bloody_vingette");

    monitor = ye_get_entity_by_name("monitor");
    monitor_graph = ye_get_entity_by_name("monitor_graph");
    monitor_heartrate = ye_get_entity_by_name("monitor_heartrate");
    SDL_SetTextureColorMod(monitor_graph->renderer->texture, 0, 255, 0); // make it bright green in engine

    text_box = ye_get_entity_by_name("text_box");
    text_content = ye_get_entity_by_name("text_content");
    text_continue_prompt = ye_get_entity_by_name("text_continue_prompt");

    // setup timer for monitor
    struct ye_timer * t = malloc(sizeof(struct ye_timer));
    t->callback = heartrate_monitor_poll;
    t->length_ms = hr_poll_interval;
    t->loops = 0;
    t->start_ticks = SDL_GetTicks();
    ye_register_timer(t);

    // other monitor timer that makes heartrate vary a little bit
    randomize_heartrate_text();

    // film grain

    unsigned char buff[640*360*3];
    int x, y;

    for(x = 0; x < 20; x++)  // create 20 noise textures to draw over screen
    {
        for(y = 0; y < 640*360; y++)
        {
            unsigned char c = rand()%255;
            buff[y*3] = c;    // red
            buff[y*3+1] = c;    // green
            buff[y*3+2] = c;    // blue
        }

        // create texture  and set its blending properties
        tex_noise[x] = SDL_CreateTexture(YE_STATE.runtime.renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STATIC, 640, 360);
        SDL_UpdateTexture(tex_noise[x], 0, buff, 640*3);
        SDL_SetTextureBlendMode(tex_noise[x], SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(tex_noise[x], 20); // set strength of texture blend from 0-255
    }
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

    // attack
    free(attack_text->renderer->renderer_impl.text->text);
    if(sd.discovered_attack_bind)
        attack_text->renderer->renderer_impl.text->text = keycode_to_label(sd.bind_attack);
    else
        attack_text->renderer->renderer_impl.text->text = strdup("?");
    ye_update_renderer_component(attack_text);

    // printf("refreshed bind labels\n");
}

/*
    note:
    for a really stupid reason (the animation being referenced at the root of one atlas image, not allowing us to conditionally tint per instance of its usage)
    we need a seperate animation meta and source to be in the cache for each key (this could be patched in engine with a renderer tint field but i dont want
    to spend more time than I need to when I can just patch it later)
*/
void ui_controller_update_bind_ui(struct bind_state_data * sd, float x, float y){
    /*
        Move up
    */
    if(sd->moving_up){
        SDL_SetTextureColorMod(up_key->renderer->texture, 255, 0, 0);
    }
    else{
        SDL_SetTextureColorMod(up_key->renderer->texture, 255, 255, 255);
    }

    /*
        Move down
    */
    if(sd->moving_down){
        SDL_SetTextureColorMod(down_key->renderer->texture, 255, 0, 0);
    }
    else{
        SDL_SetTextureColorMod(down_key->renderer->texture, 255, 255, 255);
    }

    /*
        Move left
    */
    if(sd->moving_left){
        SDL_SetTextureColorMod(left_key->renderer->texture, 255, 0, 0);
    }
    else{
        SDL_SetTextureColorMod(left_key->renderer->texture, 255, 255, 255);
    }

    /*
        Move right
    */
    if(sd->moving_right){
        SDL_SetTextureColorMod(right_key->renderer->texture, 255, 0, 0);
    }
    else{
        SDL_SetTextureColorMod(right_key->renderer->texture, 255, 255, 255);
    }

    /*
        Attack
    */
    if(sd->attacking){
        SDL_SetTextureColorMod(attack_key->renderer->texture, 255, 0, 0);
    }
    else{
        SDL_SetTextureColorMod(attack_key->renderer->texture, 255, 255, 255);
    }

    /*
        If we discoevered a bind this frame, refresh the bind labels
    */
    if(sd->discovered_bind_this_frame){
        ui_refresh_bind_labels(*sd);
        sd->discovered_bind_this_frame = false;
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
    attack_key->transform->x = x;
    attack_key->transform->y = y;

    up_text->transform->x = x;
    up_text->transform->y = y;
    down_text->transform->x = x;
    down_text->transform->y = y;
    left_text->transform->x = x;
    left_text->transform->y = y;
    right_text->transform->x = x;
    right_text->transform->y = y;
    attack_text->transform->x = x;
    attack_text->transform->y = y;

    vignette->transform->x = x;
    vignette->transform->y = y;
    bloody_vignette->transform->x = x;
    bloody_vignette->transform->y = y;
    darken->transform->x = x;
    darken->transform->y = y;

    monitor->transform->x = x;
    monitor->transform->y = y;
    monitor_graph->transform->x = x;
    monitor_graph->transform->y = y;
    monitor_heartrate->transform->x = x;
    monitor_heartrate->transform->y = y;

    text_box->transform->x = x;
    text_box->transform->y = y;
    text_content->transform->x = x;
    text_content->transform->y = y;
    text_continue_prompt->transform->x = x;
    text_continue_prompt->transform->y = y;
}

/*
    Post processing like film grain
*/
void ui_controller_additional_render(){
    // film grain
    SDL_RenderCopy(YE_STATE.runtime.renderer, tex_noise[rand()%20], 0, 0);
}

bool transforming_ui = false;
int transform_start = 0;
int transform_end = 0;
int transform_duration = 5000;
int transform_fade_in = 500;
int transform_fade_out = 2000;
int transform_poll_interval = 10;

void ui_transform_routine(){
    // printf("reached timer routine\n");
    int current_time = SDL_GetTicks() - transform_start; // offset so we just view times as relative to animation
    
    // if we are done transforming
    if(current_time > transform_end){
        transforming_ui = false;
        bloody_vignette->active = false;
        set_heartrate(80);
        return;
    }

    /*
        0ms     - 0 / 500   = 255 * 0.0
        100ms   - 100 / 500 = 255 * 0.2
        200ms   - 200 / 500 = 255 * 0.4
    */

    // float percent_through_animation = (float)( (float)current_time / (float)transform_end);

    // printf("p: %f\n",percent_through_animation);

    int res;

    // during fade in, fade it in to 100% opacity over the fade in duration
    if(current_time < transform_fade_in){
        // printf("%d/%d\n",current_time,transform_fade_in);
        float percent_through_fade_in = (float)( (float)current_time / (float)(transform_fade_in));
        // printf("percent through fade in: %f\n",percent_through_fade_in);

        // int alpha = (int)(255 * (float)(current_time / (transform_start + transform_fade_in)));
        int alpha = (int)((float)255 * percent_through_fade_in);
        // printf("fading in: %d\n",alpha);
        // res = SDL_SetTextureAlphaMod(bloody_vignette->renderer->texture,alpha); // ugly cast but whateva
        bloody_vignette->renderer->alpha = alpha;
    }
    // between fade in and out when we are supposed to be full opacity
    else if(current_time > transform_fade_in && current_time < transform_end - transform_fade_out){
        int alpha = 255;
        // printf("sustain: %d\n",alpha);
        // res = SDL_SetTextureAlphaMod(bloody_vignette->renderer->texture, alpha);
        bloody_vignette->renderer->alpha = alpha;
    }
    // fade out
    else{
        float time_until_fade_starts = transform_end - transform_fade_out;
        float percent_fade_out = (float)( ((float)current_time - time_until_fade_starts) / ((float)(transform_end) - time_until_fade_starts));

        // printf("p fade out: %f\n",percent_fade_out);

        int alpha = 255 - (int)((float)255 * percent_fade_out);
        // printf("fading out: %d\n",alpha);
        // res = SDL_SetTextureAlphaMod(bloody_vignette->renderer->texture, alpha); // ugly cast but whateva
        bloody_vignette->renderer->alpha = alpha;
    }

    // if(res == -1)
        // printf("FUCK\n");
    
    // int v; SDL_GetTextureAlphaMod(bloody_vignette->renderer->texture,&v);
    // printf("current alpha is %d\n",v);

    // re schedule timer again (we do this to elimating culumatative cpu scheduler related time loss)
    struct ye_timer * t = malloc(sizeof(struct ye_timer));
    t->callback = ui_transform_routine;
    t->length_ms = transform_poll_interval; // poll every 100ms
    t->loops = 0;
    t->start_ticks = SDL_GetTicks();
    ye_register_timer(t);
}

void begin_ui_transform_blood(){
    if(transforming_ui)
        return;
    
    set_heartrate(200);
    transforming_ui = true;
    bloody_vignette->active = true;
    bloody_vignette->renderer->alpha = 0; // set here because it wont update until first timer run (longer than one frame)

    transform_start = SDL_GetTicks();
    transform_end = transform_duration;

    struct ye_timer * t = malloc(sizeof(struct ye_timer));
    t->callback = ui_transform_routine;
    t->length_ms = transform_poll_interval; // poll every 100ms
    t->loops = 0;
    t->start_ticks = SDL_GetTicks();
    ye_register_timer(t);
    // printf("registered timer\n");

    SDL_SetTextureAlphaMod(bloody_vignette->renderer->texture, 0);
}

// text box state (dynamic)
bool displaying_text = false;
char **dialog_list = NULL;
int number_of_dialog_lines = 0;
bool ready_to_continue = false;
// int last_dialog_ticks = 0;
int char_index_into_str = 0;
// int len_of_line = 0;
int current_line = 0;
void (*end_dialog_callback)();

// static text box state
char character_noises[3][25] = {"sfx/keypress/1.mp3","sfx/keypress/2.mp3","sfx/keypress/3.mp3"};
int ms_per_character = 50;

void dialog_box_poll(){
    if(!displaying_text)
        return;
    

    if(char_index_into_str < strlen(dialog_list[current_line])){
        // printf("current line:\n%s\n",dialog_list[current_line]);
        // printf("current character index: %d\n",char_index_into_str);

        char substr[1024];
        strncpy(substr,dialog_list[current_line],char_index_into_str + 1);
        substr[char_index_into_str + 1] = '\0';
        // printf("substr: %s\n",substr);

        free(text_content->renderer->renderer_impl.text->text);
        text_content->renderer->renderer_impl.text->text = strdup(substr);
        ye_update_renderer_component(text_content);

        char_index_into_str++;
        int r = rand() % 3;
        ye_play_sound(character_noises[r],0,0.75f);
    }
    else{
        text_continue_prompt->active = true;
        ready_to_continue = true;
    }

    if(current_line > number_of_dialog_lines){
        // ----- cleanup -----
        for(int i = 0; i < number_of_dialog_lines; i++){
            free(dialog_list[i]);
            dialog_list[i] = NULL;
        }
        free(dialog_list);
        dialog_list = NULL;
        return;
    }

    // reschedule timer
    struct ye_timer * t = malloc(sizeof(struct ye_timer));
    t->callback = dialog_box_poll;
    t->length_ms = ms_per_character;
    t->loops = 0;
    t->start_ticks = SDL_GetTicks();
    ye_register_timer(t);
}

void ui_progress_dialog(){
    if(ready_to_continue){
        ye_play_sound("sfx/cont.wav",0,0.3f);

        current_line++;
        char_index_into_str = 0;
        ready_to_continue = false;
        text_continue_prompt->active = false;

        // terminate if this was last line
        if(current_line >= number_of_dialog_lines){
            displaying_text = false;
            text_box->active = false;
            text_content->active = false;
            player_controller_active = true;

            if(end_dialog_callback != NULL)
                end_dialog_callback();
        }
    }
}

void begin_dialog(char **tmp_dialog, int num_lines, void(*callback)()){
    // tmp_dialog = *tmp_dialog; // prevent decay
    // printf("Recieved: (size %d)\n",num_lines);
    // for(int i = 0; i < num_lines; i++){
        // printf("%s\n",tmp_dialog[i]);
    // }
    
    if(displaying_text)
        return;
    
    player_controller_active = false;

    text_box->active = true;

    if(callback != NULL)
        end_dialog_callback = callback;
    else
        end_dialog_callback = NULL;

    // reset the content so its not showing last text for a few frames
    free(text_content->renderer->renderer_impl.text->text);
    text_content->renderer->renderer_impl.text->text = strdup(" ");
    ye_update_renderer_component(text_content);

    text_content->active = true;
    // text_continue_prompt->active = true;

    number_of_dialog_lines = num_lines;
    ready_to_continue = false;
    displaying_text = true;
    char_index_into_str = 0;
    current_line = 0;

    dialog_list = malloc(num_lines * sizeof(char*)); // malloc the list holding pointers

    // malloc each line itself
    for(int i = 0; i < num_lines; i++){
        dialog_list[i] = strdup(tmp_dialog[i]);
    }

    // printf("took in char ** of line size %d\n",num_lines);
    // for(int i = 0; i < num_lines; i++){
        // printf("%s\n",dialog_list[i]);
    // }

    // schedule poll timer
    struct ye_timer * t = malloc(sizeof(struct ye_timer));
    t->callback = dialog_box_poll;
    t->length_ms = ms_per_character;
    t->loops = 0;
    t->start_ticks = SDL_GetTicks();
    ye_register_timer(t);
}

void set_fonts_and_colors_dialog(const char *font, const char *color, int font_size){
    free(text_content->renderer->renderer_impl.text->font_name);
    text_content->renderer->renderer_impl.text->font_name = strdup(font);

    free(text_content->renderer->renderer_impl.text->color_name);
    text_content->renderer->renderer_impl.text->color_name = strdup(color);

    text_content->renderer->renderer_impl.text->font_size = font_size;

    ye_update_renderer_component(text_content);

    free(text_box->renderer->renderer_impl.animation->meta_file);
    text_box->renderer->renderer_impl.animation->meta_file = strdup("animations/textbox/textbox.yoyo");
    ye_update_renderer_component(text_box);

    free(text_continue_prompt->renderer->renderer_impl.text->color_name);
    text_continue_prompt->renderer->renderer_impl.text->color_name = strdup("white");
    ye_update_renderer_component(text_continue_prompt);

    text_box->renderer->rect = (struct ye_rectf){-225,290,671,323};
}

void reset_fonts_and_colors_dialog(){
    free(text_content->renderer->renderer_impl.text->font_name);
    text_content->renderer->renderer_impl.text->font_name = strdup("satan");

    free(text_content->renderer->renderer_impl.text->color_name);
    text_content->renderer->renderer_impl.text->color_name = strdup("red");

    text_content->renderer->renderer_impl.text->font_size = 57;

    ye_update_renderer_component(text_content);

    free(text_box->renderer->renderer_impl.animation->meta_file);
    text_box->renderer->renderer_impl.animation->meta_file = strdup("animations/red_textbox/red_textbox.yoyo");
    ye_update_renderer_component(text_box);

    free(text_continue_prompt->renderer->renderer_impl.text->color_name);
    text_continue_prompt->renderer->renderer_impl.text->color_name = strdup("red");
    ye_update_renderer_component(text_continue_prompt);

    text_box->renderer->rect = (struct ye_rectf){-225,290,671,323};
}