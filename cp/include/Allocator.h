#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

typedef struct Allocator {
    void *memory; 
    size_t memory_size;    

    void* (*alloc)(struct Allocator*, size_t);

    void (*free)(struct Allocator*, void*);
} Allocator;

#endif 