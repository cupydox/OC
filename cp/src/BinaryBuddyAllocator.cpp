#include "../include/Allocator.h"
#include <stddef.h>
#include <cmath>
#include <iostream>
#include <cstring>

class BinaryBuddyAllocator {
private:
    struct Block {
        size_t size;
        Block* next;
    };

    void* memory;
    size_t totalSize;
    Block* freeList[32];

    size_t log2(size_t size) {
        return static_cast<size_t>(std::log2(size));
    }

    void allocatePage(size_t blockSize, int index) {
        if (blockSize > totalSize) return;

        Block* page = (Block*)memory;
        memset(page, 0, totalSize);
        size_t numBlocks = totalSize / blockSize;

        for (size_t i = 0; i < numBlocks; i++) {
            Block* newBlock = (Block*)((char*)page + i * blockSize);
            newBlock->size = blockSize;
            newBlock->next = freeList[index];
            freeList[index] = newBlock;
        }
    }

public:
    BinaryBuddyAllocator(void *memory, size_t size) : memory(memory), totalSize(size) {
        for (int i = 0; i < 32; i++) {
            freeList[i] = nullptr;
        }
    }

    void* alloc(size_t size) {
        if (size == 0 || size > totalSize) return nullptr;

        size_t blockSize = 1;
        while (blockSize < size) blockSize <<= 1;
        int index = log2(blockSize);

        if (freeList[index] == nullptr) {
            allocatePage(blockSize, index);
        }

        Block* block = freeList[index];
        if (block) {
            freeList[index] = block->next;
            return (void*)((char*)block + sizeof(Block));
        }

        return nullptr;
    }

    void free(void* ptr) {
        if (ptr == nullptr || (char*)ptr < (char*)memory || (char*)ptr >= (char*)memory + totalSize) return;

        Block* block = (Block*)((char*)ptr - sizeof(Block));
        int index = log2(block->size);

        block->next = freeList[index];
        freeList[index] = block;
    }
};

Allocator* createBinaryBuddyAllocator(void* realMemory, size_t memory_size) {
    Allocator* iface = new Allocator();
    iface->memory = realMemory;
    iface->memory_size = memory_size;

    iface->alloc = [](Allocator* allocator, size_t size) -> void* {
        BinaryBuddyAllocator allocatorInstance(allocator->memory, allocator->memory_size);
        return allocatorInstance.alloc(size);
    };

    iface->free = [](Allocator* allocator, void* ptr) {
        BinaryBuddyAllocator allocatorInstance(allocator->memory, allocator->memory_size);
        allocatorInstance.free(ptr);
    };

    return iface;
}
