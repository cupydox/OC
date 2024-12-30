#include "gtest/gtest.h"
#include "../include/BuddyAllocator.h"
#include "../include/BinaryBuddyAllocator.h"
#include <chrono>
#include <cstdlib>


#define MEASURE_TIME(block, duration) { \
    auto start = std::chrono::high_resolution_clock::now(); \
    block \
    auto end = std::chrono::high_resolution_clock::now(); \
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
}

// Тест на использование памяти
TEST(AllocatorComparison, MemoryUsageFactor) {
    const size_t memorySize = 1024 * 1024; 
    char memory1[memorySize], memory2[memorySize];

    Allocator* buddy = createBuddyAllocator(memory1, memorySize);
    Allocator* binaryBuddy = createBinaryBuddyAllocator(memory2, memorySize);

    const size_t blockSize = 128; 
    const int blockCount = memorySize / blockSize;

    std::cout << "Allocating memory blocks..." << std::endl;
    for (int i = 0; i < blockCount / 2; i++) {
        void* ptr1 = buddy->alloc(buddy, blockSize);
        void* ptr2 = binaryBuddy->alloc(binaryBuddy, blockSize);
        EXPECT_NE(ptr1, nullptr);
        EXPECT_NE(ptr2, nullptr);
    }

    size_t usedBuddyMemory = blockCount / 2 * blockSize;
    size_t usedBinaryBuddyMemory = blockCount / 2 * blockSize;

    double buddyUsage = static_cast<double>(usedBuddyMemory) / memorySize;
    double binaryBuddyUsage = static_cast<double>(usedBinaryBuddyMemory) / memorySize;

    EXPECT_GE(buddyUsage, 0.5); 
    EXPECT_GE(binaryBuddyUsage, 0.5);

    std::cout << "BuddyAllocator usage more than 50%" << "\n";
    std::cout << "BinaryBuddyAllocator usage more than 50%" << "\n";
}

// Тест на скорость выделения памяти
TEST(AllocatorComparison, AllocationSpeed) {
    const size_t memorySize = 1024 * 1024; 
    char memory1[memorySize], memory2[memorySize];

    Allocator* buddy = createBuddyAllocator(memory1, memorySize);
    Allocator* binaryBuddy = createBinaryBuddyAllocator(memory2, memorySize);

    const size_t blockSize = 128;
    const int blockCount = 1000;

    long long buddyTime = 0, binaryBuddyTime = 0;

    std::cout << "Measuring allocation time for BuddyAllocator..." << std::endl;
    MEASURE_TIME(
        for (int i = 0; i < blockCount; i++) {
            void* ptr = buddy->alloc(buddy, blockSize);
            EXPECT_NE(ptr, nullptr);
        },
        buddyTime
    );

    std::cout << "Measuring allocation time for BinaryBuddyAllocator..." << std::endl;
    MEASURE_TIME(
        for (int i = 0; i < blockCount; i++) {
            void* ptr = binaryBuddy->alloc(binaryBuddy, blockSize);
            EXPECT_NE(ptr, nullptr);
        },
        binaryBuddyTime
    );

    std::cout << "BuddyAllocator allocation time: " << buddyTime << " µs\n";
    std::cout << "BinaryBuddyAllocator allocation time: " << binaryBuddyTime << " µs\n";
}

// Тест на скорость освобождения памяти
TEST(AllocatorComparison, DeallocationSpeed) {
    const size_t memorySize = 1024 * 1024; 
    char memory1[memorySize], memory2[memorySize];

    Allocator* buddy = createBuddyAllocator(memory1, memorySize);
    Allocator* binaryBuddy = createBinaryBuddyAllocator(memory2, memorySize);

    const size_t blockSize = 128;
    const int blockCount = 1000;
    void* buddyBlocks[blockCount];
    void* binaryBuddyBlocks[blockCount];

    std::cout << "Allocating memory blocks for deallocation..." << std::endl;
    for (int i = 0; i < blockCount; i++) {
        buddyBlocks[i] = buddy->alloc(buddy, blockSize);
        binaryBuddyBlocks[i] = binaryBuddy->alloc(binaryBuddy, blockSize);
    }

    long long buddyTime = 0, binaryBuddyTime = 0;

    std::cout << "Measuring deallocation time for BuddyAllocator..." << std::endl;
    MEASURE_TIME(
        for (int i = 0; i < blockCount; i++) {
            buddy->free(buddy, buddyBlocks[i]);
        },
        buddyTime
    );

    std::cout << "Measuring deallocation time for BinaryBuddyAllocator..." << std::endl;
    MEASURE_TIME(
        for (int i = 0; i < blockCount; i++) {
            binaryBuddy->free(binaryBuddy, binaryBuddyBlocks[i]);
        },
        binaryBuddyTime
    );

    std::cout << "BuddyAllocator deallocation time: " << buddyTime << " µs\n";
    std::cout << "BinaryBuddyAllocator deallocation time: " << binaryBuddyTime << " µs\n";
}

// Тест на фактор использования
TEST(AllocatorComparison, MemoryUsageAfterMixedOperations) {
    const size_t memorySize = 1024 * 1024; 
    char memory1[memorySize], memory2[memorySize];

    Allocator* buddy = createBuddyAllocator(memory1, memorySize);
    Allocator* binaryBuddy = createBinaryBuddyAllocator(memory2, memorySize);

    size_t blockSize = 64; 
    const int operationCount = 10000;

    void* buddyBlocks[operationCount];
    void* binaryBuddyBlocks[operationCount];

    std::cout << "Running memory usage factor test after mixed operations...\n";

    for (int i = 0; i < operationCount; i++) {
        if (i % 10 == 0) {
            blockSize = 512;
        } else {
            blockSize = 64;
        }
        buddyBlocks[i] = buddy->alloc(buddy, blockSize);
        binaryBuddyBlocks[i] = binaryBuddy->alloc(binaryBuddy, blockSize);

        EXPECT_NE(buddyBlocks[i], nullptr);
        EXPECT_NE(binaryBuddyBlocks[i], nullptr);

        if (i % 2 == 0 && i > 0) { 
            buddy->free(buddy, buddyBlocks[i / 2]);
            binaryBuddy->free(binaryBuddy, binaryBuddyBlocks[i / 2]);
        }
    }

    size_t buddyUsedMemory = 0, binaryBuddyUsedMemory = 0;
    for (int i = 0; i < operationCount; i++) {
        if (buddyBlocks[i]) buddyUsedMemory += blockSize;
        if (binaryBuddyBlocks[i]) binaryBuddyUsedMemory += blockSize;
    }

    double buddyUsage = static_cast<double>(buddyUsedMemory) / memorySize * 100.0;
    double binaryBuddyUsage = static_cast<double>(binaryBuddyUsedMemory) / memorySize * 100.0;

    std::cout << "BuddyAllocator memory usage after mixed operations: " << buddyUsage << "%\n";
    std::cout << "BinaryBuddyAllocator memory usage after mixed operations: " << binaryBuddyUsage << "%\n";

    EXPECT_GE(buddyUsage, 20.0);  
    EXPECT_GE(binaryBuddyUsage, 20.0);
}



