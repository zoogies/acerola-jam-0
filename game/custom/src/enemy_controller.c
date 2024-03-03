#include <math.h>
// hack to disable error (at compile time this is defined by libm):
#ifndef M_PI
#define M_PI 3.14
#endif

#include <yoyoengine/yoyoengine.h>

#include "enemy_controller.h"
#include "player_controller.h"

struct ye_entity *en_player_ent = NULL;

struct game_enemy *enemies = NULL;

void enemy_controller_init(){
    // printf("enemy controller init\n");
    en_player_ent = ye_get_entity_by_name("PLAYER");

    // consider getting all entities with tag "enemy" so we
    // can create them in editor and not spawn them in through code

    struct ye_entity_node *itr = tag_list_head;
    while (itr != NULL){
        /*
            If we have an entity tagged "enemy", register it
        */
        if(entity_has_tag(itr->entity, "enemy")){
            struct game_enemy *en = malloc(sizeof(struct game_enemy));
            en->ent = itr->entity;
            en->next = enemies;
            enemies = en;
            // printf("added enemy from tag\n");
        }

        itr = itr->next;
    }
}

void enemy_controller_update(){
    struct game_enemy *itr = enemies;
    while(itr != NULL){

        // TODO: most of these checks should only be happening for enemies
        // close to the player, maybe we have triggers that spawn enemies
        // and just keep doing for all. idk the spawning situation yet

        /*
            Update rotation to look at player
        */
        // find angle between enemy and player
        int ex = itr->ent->transform->x;
        int ey = itr->ent->transform->y;
        int px = en_player_ent->transform->x;
        int py = en_player_ent->transform->y;
        double angle = atan2( ey - py, ex - px ) * (180.0 / M_PI) - 90;
        itr->ent->renderer->rotation = (float)angle;

        // now that we have the direction of the player, update this enemy to move towards them
        float xv = 0;
        float yv = 0;
        // player is left of enemy
        if(ex - px > 0){
            xv = -100.0f;
            // printf("player is left of enemy\n");
        }
        else{
            xv = 100.0f;
            // printf("player is right of enemy\n");
        }

        // player is above enemy
        if(ey - py > 0){
            yv = -100.0f;
            // printf("player is above enemy\n");
        }
        else{
            yv = 100.0f;
            // printf("player is below enemy\n");
        }

        itr->ent->physics->velocity.x = xv;
        itr->ent->physics->velocity.y = yv;

        itr = itr->next;
    }    
}

void enemy_controller_shutdown(){
    // printf("enemy controller shutdonw\n");
    en_player_ent = NULL;

    struct game_enemy *itr = enemies;
    while(itr != NULL){
        struct game_enemy *tmp = itr;
        itr = itr->next;
        free(tmp);
    }
}

void create_new_enemy(float x, float y){

}
