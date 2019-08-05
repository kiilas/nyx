#include "nyx/nyx.h"

struct NYX_LIST {
    size_t  elem_size;
    size_t  len;
    size_t  capacity;
    void   *data;
};

static int set_capacity(NYX_LIST *list, size_t capacity) {
    void *new_data;
    
    if(list->len > capacity)
        return -1;
    new_data = realloc(list->data, capacity * list->elem_size);
    if(!new_data)
        return -1;
    list->capacity = capacity;
    list->data = new_data;
    return 0;
}

static int ensure_capacity(NYX_LIST *list, size_t min) {
    if(list->capacity >= min)
        return 0;
    if(min > 2 * list->capacity)
        return set_capacity(list, min);
    return set_capacity(list, list->capacity * 2);
}

static int grow_to(NYX_LIST *list, size_t len) {
    if(list->len >= len)
        return 0;
    if(ensure_capacity(list, len))
        return -1;
    list->len = len;
    return 0;
}

static int grow(NYX_LIST *list) {
    return grow_to(list, list->len + 1);
}

size_t _nyx_list_index_sorted(const NYX_LIST *list, const void *elem, int *exists) {
    size_t min_idx = 0;
    size_t max_idx = list->len;

    while(min_idx < max_idx)
    {
        size_t avg = (min_idx + max_idx) / 2;
        const void *pivot = nyx_list_get_ptr_unsafe(list, avg);
        int cmp = memcmp(elem, pivot, list->elem_size);

        if(cmp < 0)
            max_idx = avg;
        else if(!cmp)
        {
            if(exists)
                *exists = 1;
            return avg;
        }
        else
            min_idx = avg + 1;
    }
    if(exists)
        *exists = 0;
    return min_idx;
}

NYX_LIST *nyx_list_init(size_t elem_size) {
    NYX_LIST *list;

    list = calloc(1, sizeof(NYX_LIST));
    if(!list)
        return 0;
    list->elem_size = elem_size;
    return list;
}

void nyx_destroy_list(NYX_LIST *list) {
    if(!list)
        return;
    free(list->data);
    free(list);
}

void *nyx_list_get_ptr_unsafe(const NYX_LIST *list, size_t idx) {
    return (char *)list->data + idx*list->elem_size;
}

void nyx_list_set_unsafe(NYX_LIST *list, size_t idx, const void *elem) {
    memcpy(nyx_list_get_ptr_unsafe(list, idx), elem, list->elem_size);
}

int nyx_list_insert(NYX_LIST *list, size_t idx, const void *elem) {
    if(idx > list->len)
        return -1;
    if(grow(list))
       return -1;
    memmove(nyx_list_get_ptr_unsafe(list, idx + 1),
            nyx_list_get_ptr_unsafe(list, idx),
            (list->len-idx-1) * list->elem_size);
    memcpy(nyx_list_get_ptr_unsafe(list, idx), elem, list->elem_size);
    return 0;
}

int nyx_list_insert_sorted(NYX_LIST *list, const void *elem) {
    size_t idx;
    
    idx = _nyx_list_index_sorted(list, elem, 0);
    return nyx_list_insert(list, idx, elem);
}

int nyx_list_insert_sorted_unique(NYX_LIST *list, const void *elem) {
    size_t idx;
    int exists;

    idx = _nyx_list_index_sorted(list, elem, &exists);
    if(!exists)
        return nyx_list_insert(list, idx, elem);
    return 0;
}

int nyx_list_remove(NYX_LIST *list, size_t idx) {
    if(idx >= list->len)
        return -1;
    --list->len;
    memmove(nyx_list_get_ptr_unsafe(list, idx),
            nyx_list_get_ptr_unsafe(list, idx+1),
            (list->len-idx) * list->elem_size);
    return 0;
}
