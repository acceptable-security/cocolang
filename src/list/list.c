#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "list.h"

// Allocate memory for a list
list_t* list_new() {
    list_t* list = (list_t*) malloc(sizeof(list_t));

    if ( list == NULL ) {
        return list;
    }

    list->length = 0;
    list->alloc_size = LIST_SIZE;

    list->data = (void**) malloc(sizeof(void*) * list->alloc_size);

    if ( list->data == NULL ) {
        free(list);
        return NULL;
    }

    return list;
}

// Grow the list internal memory
bool list_grow(list_t* list, uint64_t amount) {
    assert(list);

    list->alloc_size += amount;
    void** tmp = (void**) realloc(list->data, list->alloc_size);

    if ( tmp == NULL ) {
        free(list->data);
        list->data = NULL;

        return false;
    }

    list->data = tmp;
    return true;
}

// Append a pointer to the end of the list
bool list_append(list_t* list, void* data) {
    assert(list);
    assert(list->data);

    list->length++;

    if ( list->length >= list->alloc_size ) {
        if ( !list_grow(list, LIST_SIZE) ) {
            return false;
        }
    }

    list->data[list->length++] = data;
    return true;
}

// Deallocate a list
void list_free(list_t* list) {
    assert(list);

    if ( list->data ) {
        free(list->data);
    }

    free(list);
}
