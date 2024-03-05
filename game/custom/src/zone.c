
#include <yoyoengine/yoyoengine.h>

void _lock_zone_one(struct ye_entity *ent){
    // printf("locking: %s\n",ent->name);
    ent->active = true;
}

void lock_zone_one(){
    ye_for_matching_tag("black_zone_1",_lock_zone_one);
}

void _unlock_zone_one(struct ye_entity *ent){
    ent->active = false;
}

void unlock_zone_one(){
    ye_for_matching_tag("black_zone_1",_unlock_zone_one);
}
