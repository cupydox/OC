#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include "Allocator.h"
#include <stddef.h>

class BuddyAllocator:public Allocator {
private:
    struct BuddyBlock {
        size_t size;
        BuddyBlock* next;
    };

    void* memory;
    size_t totalSize;
    BuddyBlock* freeList[32]; 

    size_t log2(size_t size);

public:
    BuddyAllocator(void* memory, size_t size);
    void* alloc(size_t size);
    void free(void* ptr);
};

Allocator* createBuddyAllocator(void* realMemory, size_t memory_size);

#endif 