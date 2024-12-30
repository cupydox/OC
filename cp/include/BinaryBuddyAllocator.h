#ifndef BINARY_BUDDY_ALLOCATOR_H
#define BINARY_BUDDY_ALLOCATOR_H

#include "Allocator.h"
#include <stddef.h>

class BinaryBuddyAllocator:public Allocator {
private:
    struct BinaryBuddyBlock {
        size_t size;
        BinaryBuddyBlock* left;
        BinaryBuddyBlock* right;
    };

    void* memory;
    size_t totalSize;
    BinaryBuddyBlock* root;

    size_t log2(size_t size);
    BinaryBuddyBlock* split(BinaryBuddyBlock* block, size_t size);
    BinaryBuddyBlock* merge(BinaryBuddyBlock* block);

public:
    BinaryBuddyAllocator(void* memory, size_t size);
    void* alloc(size_t size);
    void free(void* ptr);
};

Allocator* createBinaryBuddyAllocator(void* realMemory, size_t memory_size);

#endif 
