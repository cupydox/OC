#include <iostream>
#include <sstream>
#include "../include/math_interface.h"

int main() {
    std::string command;
    while (true) {
        std::cout << "Enter command (1 for E, 2 for BubbleSort, 0 to exit): ";
        std::getline(std::cin, command);

        std::istringstream iss(command);
        int choice;
        iss >> choice;

        if (choice == 0) {
            break; 
        } else if (choice == 1) {
            int x;
            iss >> x;
            std::cout << "E: " << E(x) << std::endl;
        } else if (choice == 2) {
            int size;
            iss >> size;
            int *array = new int[size];
            for (int i = 0; i < size; ++i) {
                iss >> array[i];
            }

            BubbleSort(array, size);
            std::cout << "Sorted array: ";
            for (int i = 0; i < size; ++i) {
                std::cout << array[i] << " ";
            }
            std::cout << std::endl;

            delete[] array;
        } else {
            std::cout << "Unknown command!" << std::endl;
        }
    }
    return 0;
}