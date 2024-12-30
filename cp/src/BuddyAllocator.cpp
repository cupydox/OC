#include "../include/BuddyAllocator.h"
#include "../include/Allocator.h"
#include <stddef.h>
#include <cmath>
#include <iostream>

BuddyAllocator::BuddyAllocator(void *memory, size_t size) : memory(memory), totalSize(size) {
    for (int i = 0; i < 32; i++) {
        freeList[i] = nullptr;
    }
    BuddyBlock *block = (BuddyBlock*)memory;
    block->size = size;
    block->next = nullptr;
    freeList[log2(size)] = block;
}

size_t BuddyAllocator::log2(size_t size) {
    return static_cast<size_t>(std::log2(size));
}

void* BuddyAllocator::alloc(size_t size) {
    if (size == 0 || size > totalSize) return nullptr;

    size_t blockSize = 1;
    while (blockSize < size) blockSize <<= 1;
    int index = log2(blockSize);

    if (freeList[index] != nullptr) {
        BuddyBlock *block = freeList[index];
        freeList[index] = block->next;
        return (void*)((char*)block + sizeof(BuddyBlock));
    }

    for (int i = index + 1; i < 32; i++) {
        if (freeList[i] != nullptr) {
            BuddyBlock *block = freeList[i];
            freeList[i] = block->next;

            while (i > index) {
                i--;
                BuddyBlock *buddy = (BuddyBlock*)((char*)block + (block->size / 2));
                buddy->size = block->size / 2;
                buddy->next = freeList[i];
                freeList[i] = buddy;

                block->size /= 2;
            }

            return (void*)((char*)block + sizeof(BuddyBlock));
        }
    }

    return nullptr;
}

void BuddyAllocator::free(void *ptr) {
    if (ptr == nullptr || (char*)ptr < (char*)memory || (char*)ptr >= (char*)memory + totalSize) return;

    BuddyBlock *block = (BuddyBlock*)((char*)ptr - sizeof(BuddyBlock));
    size_t blockSize = block->size;
    int index = log2(blockSize);

    while (true) {
        BuddyBlock *buddy = (BuddyBlock*)((char*)block + blockSize); // Определяем адрес "двойника"
        BuddyBlock **current = &freeList[index];
        bool foundBuddy = false;

        while (*current) {
            if (*current == buddy) {  // Если "двойник" найден в списке свободных блоков
                foundBuddy = true;
                *current = buddy->next;  // Удаляем "двойника" из списка
                break;
            }
            current = &(*current)->next;
        }

        if (foundBuddy) {
            block = (block < buddy) ? block : buddy;  // Определяем новый блок как тот, что с меньшим адресом
            block->size *= 2;  // Увеличиваем размер блока
            index++;
        } else {
            break;  // Если "двойник" не найден, выходим из цикла
        }
    }

    block->next = freeList[index];
    freeList[index] = block;
}


Allocator* createBuddyAllocator(void* realMemory, size_t memory_size) {
    Allocator* iface = new Allocator();
    iface->memory = realMemory;
    iface->memory_size = memory_size;

    iface->alloc = [](Allocator* allocator, size_t size) -> void* {
        BuddyAllocator buddyAllocator(allocator->memory, allocator->memory_size);
        return buddyAllocator.alloc(size);
    };

    iface->free = [](Allocator* allocator, void* ptr) {
        BuddyAllocator buddyAllocator(allocator->memory, allocator->memory_size);
        buddyAllocator.free(ptr);
    };

    return iface;
}
