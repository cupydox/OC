#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include "Allocator.h"
#include "BuddyAllocator.h"
#include "BinaryBuddyAllocator.h"

int main() {
    // Выделяем память для аллокаторов
    size_t memorySize = 1024;  // Пример размера памяти для аллокатора
    void* memoryForBuddy = malloc(memorySize);  // Выделяем память для BuddyAllocator
    void* memoryForBinaryBuddy = malloc(memorySize);  // Выделяем память для BinaryBuddyAllocator

    if (!memoryForBuddy || !memoryForBinaryBuddy) {
        std::cerr << "Memory allocation failed!" << std::endl;
        return 1;
    }

    // Создаем аллокаторы с переданной памятью
    BuddyAllocator buddyAllocator(memoryForBuddy, memorySize);
    BinaryBuddyAllocator binaryBuddyAllocator(memoryForBinaryBuddy, memorySize);

    // Освобождаем память
    free(memoryForBuddy);
    free(memoryForBinaryBuddy);

    return 0;
}