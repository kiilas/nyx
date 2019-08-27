#ifndef NYX_MAP_H
#define NYX_MAP_H

#include <stdint.h>

typedef struct NYX_MAP NYX_MAP;

NYX_MAP *nyx_init_map(size_t key_size, size_t value_size);
void nyx_destroy_map(NYX_MAP *map);

void *nyx_map_get(const NYX_MAP *map, const void *key);
int nyx_map_insert(const NYX_MAP *map, const void *key, const void *value);

#endif
