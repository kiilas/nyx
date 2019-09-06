#ifndef NYX_MAP_H
#define NYX_MAP_H

#include <stdint.h>

typedef struct NYX_MAP NYX_MAP;

NYX_MAP *nyx_init_map(size_t key_size, size_t value_size);
void nyx_destroy_map(NYX_MAP *map);

void *nyx_map_get(const NYX_MAP *map, const void *key);
int nyx_map_insert(const NYX_MAP *map, const void *key, const void *value);
int nyx_map_remove(NYX_MAP *map, const void *key);

size_t nyx_map_size(const NYX_MAP *map);

void *nyx_map_key_by_index(const NYX_MAP *map, size_t idx);
void *nyx_map_key_by_index_unsafe(const NYX_MAP *map, size_t idx);
void *nyx_map_value_by_index(const NYX_MAP *map, size_t idx);
void *nyx_map_value_by_index_unsafe(const NYX_MAP *map, size_t idx);

#endif
