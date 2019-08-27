#include "nyx/nyx.h"

#include "_list.h"

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

void *nyx_map_get(const NYX_MAP *map, const void *key) {
    size_t idx;
    int exists;

    idx = _nyx_list_index_sorted(map->keys, key, &exists);
    if(!exists)
        return 0;
    return nyx_list_get_ptr_unsafe(map->values, idx);
}

int nyx_map_insert(const NYX_MAP *map, const void *key, const void *value) {
    size_t idx;
    int exists;

    idx = _nyx_list_index_sorted(map->keys, key, &exists);
    if(exists)
    {
        nyx_list_set_unsafe(map->values, idx, value);
        return 0;
    }
    if(nyx_list_insert(map->keys, idx, key))
        return -1;
    if(nyx_list_insert(map->values, idx, value))
    {
        nyx_list_remove(map->keys, idx);
        return -1;
    }
    return 0;
}
