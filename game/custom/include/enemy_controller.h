
#ifndef ENEMY_CONTROLLER
#define ENEMY_CONTROLLER

struct game_enemy {
    struct ye_entity *ent;
    struct game_enemy *next;
};

void enemy_controller_init();

void enemy_controller_update();

void enemy_controller_shutdown();

// api

void create_new_enemy(float x, float y);

#endif
