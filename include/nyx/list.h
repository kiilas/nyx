#ifndef NYX_LIST_H
#define NYX_LIST_H

typedef struct NYX_LIST NYX_LIST;

NYX_LIST *nyx_list_init(size_t elem_size);
void nyx_destroy_list(NYX_LIST *list);

void *nyx_list_get(const NYX_LIST *list, size_t idx);
void *nyx_list_get_unsafe(const NYX_LIST *list, size_t idx);
void nyx_list_set_unsafe(NYX_LIST *list, size_t idx, const void *elem);

int nyx_list_add(NYX_LIST *list, const void *elem);
int nyx_list_insert(NYX_LIST *list, size_t idx, const void *elem);
int nyx_list_insert_sorted(NYX_LIST *list, const void *elem);
int nyx_list_insert_sorted_unique(NYX_LIST *list, const void *elem);

int nyx_list_remove(NYX_LIST *list, size_t idx);

size_t nyx_list_size(const NYX_LIST *list);

#endif
