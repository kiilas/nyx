#include "nyx/nyx.h"

struct NYX_MAP {
    size_t key_size;
    size_t value_size;
    NYX_LIST *keys;
    NYX_LIST *values;
};

NYX_MAP *nyx_init_map(size_t key_size, size_t value_size) {
    NYX_MAP *map;

    map = calloc(1, sizeof(NYX_MAP));
    if(!map)
        return 0;
    map->keys = nyx_list_init(key_size);
    if(!map->keys)
        goto fail;
    map->values = nyx_list_init(value_size);
    if(!map->values)
        goto fail;
    return map;
fail:
    nyx_destroy_map(map);
    return 0;
}

void nyx_destroy_map(NYX_MAP *map) {
    if(!map)
        return;
    nyx_destroy_list(map->keys);
    nyx_destroy_list(map->values);
    free(map);
}

int nyx_map_insert(const NYX_MAP *map, void *key, void *value) {
    int idx;
    
    // TODO what if key already exists
    idx = nyx_list_insert_sorted(map->keys, key);
    if(idx < 0)
        return -1;
    if(nyx_list_insert(map->values, idx, value))
    {
        nyx_list_remove(map->keys, idx);
        return -1;
    }
    return 0;
}
