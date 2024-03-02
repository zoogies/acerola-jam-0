
#include <yoyoengine/yoyoengine.h>
#include "player_controller.h"

void ui_controller_attatch();

char * keycode_to_label(SDL_Keycode code);

void ui_refresh_bind_labels(struct bind_state_data sd);

void ui_controller_update_bind_ui(struct bind_state_data sd, float x, float y);
