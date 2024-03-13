#include <math.h>
// hack to disable error (at compile time this is defined by libm):
#ifndef M_PI
#define M_PI 3.14
#endif
#include <stdlib.h>

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
        if(ye_entity_has_tag(itr->entity, "enemy")){
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
    struct ye_entity *enemy = ye_create_entity_named("enemy");

    ye_add_transform_component(enemy,x,y);
    
    ye_add_image_renderer_component(enemy,50,"images/player/theriacenemy.png");
    enemy->renderer->rect = (struct ye_rectf){0,0,150,150};

    ye_add_static_collider_component(enemy,(struct ye_rectf){0,0,150,150});

    ye_add_physics_component(enemy,0,0);

    ye_add_tag(enemy, "enemy"); // also creates the tag component

    // add to tracked enemies
    struct game_enemy *en = malloc(sizeof(struct game_enemy));
    en->ent = enemy;
    en->next = enemies;
    enemies = en;
}

void destroy_enemy(struct game_enemy *en){
    // printf("destroying enemy: %s\n",en->ent->name);
    // find en in enemies and remove it
    struct game_enemy *itr = enemies;
    struct game_enemy *last = NULL;
    while(itr != NULL){
        if(itr == en){
            // enemy was first in list
            if(last == NULL){
                enemies = itr->next;
            }
            else {
                last->next = itr->next;
            }

            ye_destroy_entity(itr->ent);
            free(itr);
            break;
        }
        last = itr;
        itr = itr->next;
    }
}

/*
    Kills any enemies within the player arm swing field
*/
void kill_enemies_within(float px, float py, float pa){
    struct game_enemy *en = enemies;

    while(en != NULL){
        float ex = en->ent->transform->x + (en->ent->renderer->rect.w / 2);
        float ey = en->ent->transform->y + (en->ent->renderer->rect.h / 2);

        // printf("--------\n");

        // angle between player and enemy
        double player_to_enemy_angle = atan2( ey - py, ex - px ) * (180.0 / M_PI) + 90;
        // printf("player to enemy angle: %f\n",player_to_enemy_angle);
        // printf("player look angle: %f\n",pa);
        // printf("left bound: %f, right bound: %f\n",pa - 120, pa + 120);

        // printf("player:\n\tx:%f,%f\nenemy:\n\tx:%f,%f\n",px,py,ex,ey);
        float dist = ye_distance(px,py,ex,ey);
        // printf("distance from player to enemy: %f\n",dist);

        // if the enemy is within 180 degree field of view of where player is looking
        if(true/*pa - 120 < player_to_enemy_angle && pa + 120 > player_to_enemy_angle*/){
            // printf("enemy is within view cone\n");

            if(dist > 0 && dist < 350){
                /*
                    Play a death animation and sound
                */
                struct ye_entity *anim = ye_create_entity_named("death anim");
                ye_add_transform_component(anim, ex, ey);
                ye_add_animation_renderer_component(anim,1,"animations/death/death.yoyo");
                anim->renderer->rect.w *= 1.5;
                anim->renderer->rect.h *= 1.5;
                anim->renderer->rect.x = -(anim->renderer->rect.w / 2);
                anim->renderer->rect.y = -(anim->renderer->rect.h / 2);

                // printf("enemy is within attack range\n");
                // TODO: kill enemy
                struct game_enemy * tmp = en->next;
                destroy_enemy(en);
                en = tmp;

                // play death sound
                int r = rand() % 4;
                // printf("rolled %d\n",r);
                switch(r){
                    case 1:
                        ye_play_sound("sfx/death/1.mp3",0,1.0f);
                        break;
                    case 2:
                        ye_play_sound("sfx/death/2.mp3",0,1.0f);
                        break;
                    case 3:
                        ye_play_sound("sfx/death/3.mp3",0,1.0f);
                        break;
                    default:
                        ye_play_sound("sfx/death/4.mp3",0,1.0f);
                        break;
                }
                continue;
            }

        }

        en = en->next; // case for if we dont destroy an enemy
    }
}