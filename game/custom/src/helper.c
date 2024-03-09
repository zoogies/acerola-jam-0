/*
    Does a bunch of random shit
*/
#include <math.h>

#include <yoyoengine/yoyoengine.h>

const char blood_images[3][50] = {"images/blood/blood1.png","images/blood/blood2.png","images/blood/blood3.png"};

void splat_blood(float x, float y){
    struct ye_entity *blood = ye_create_entity_named("blood");
    
    // generate a slightly random offset and rotation
    x = x + ((rand() % 50) - 25);
    y = y + ((rand() % 50) - 25);
    float r = rand() % 360;

    ye_add_transform_component(blood,x,y);
    ye_add_image_renderer_component(blood, 1, blood_images[rand() % 3]);
    blood->renderer->rotation = r;
}