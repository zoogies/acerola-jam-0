
#include <yoyoengine/yoyoengine.h>

void _lock_zone(struct ye_entity *ent){
    ent->active = true;
}

void _unlock_zone(struct ye_entity *ent){
    ent->active = false;
}

void unlock_by_tag(const char *tag){
    ye_for_matching_tag(tag,_unlock_zone);
}

void lock_by_tag(const char *tag){
    ye_for_matching_tag(tag,_lock_zone);
}