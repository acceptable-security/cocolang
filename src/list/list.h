#include <stdint.h>

#define LIST_SIZE 64
#define list_get(L, I) (L)->data[(I)]

// List of pointer structure
typedef struct {
    void** data;            // Location where all the data is stored

    uint64_t alloc_size;    // Current size of the allocated data
    uint64_t length;        // Current index in the allocated data
} list_t;
