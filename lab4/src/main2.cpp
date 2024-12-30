#include <iostream>
#include <sstream>
#include <dlfcn.h>

int main() {
    void* handle = nullptr;
    float (*E)(int) = nullptr;
    void (*Sort)(int*, int, int) = nullptr;
    bool useMath1 = true; 

    auto loadLibrary = [&](bool useMath1) {
        if (handle) {
            dlclose(handle); 
        }

        const char* libPath = useMath1 ? "./libmath1.so" : "./libmath2.so";
        handle = dlopen(libPath, RTLD_LAZY);
        if (!handle) {
            std::cerr << "Cannot open library: " << dlerror() << std::endl;
            return false;
        }

        E = reinterpret_cast<float (*)(int)>(dlsym(handle, "E"));
        Sort = reinterpret_cast<void (*)(int*, int, int)>(dlsym(handle, "QuickSort"));

        if (!E || !Sort) {
            std::cerr << "Cannot load symbols: " << dlerror() << std::endl;
            dlclose(handle);
            handle = nullptr;
            return false;
        }

        return true;
    };

    if (!loadLibrary(useMath1)) {
        return 1;
    }

    std::string command;
    while (true) {
        std::cout << "Enter command (1 for E, 2 for QuickSort, 0 to switch library, -1 to exit): ";
        std::getline(std::cin, command);

        std::istringstream iss(command);
        int choice;
        iss >> choice;

        if (choice == -1) {
            break;
        } else if (choice == 0) {
            useMath1 = !useMath1; 
            if (!loadLibrary(useMath1)) {
                return 1;
            }
            std::cout << "Switched to " << (useMath1 ? "libmath1.so" : "libmath2.so") << std::endl;
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

            Sort(array, 0, size - 1);
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

    if (handle) {
        dlclose(handle);
    }
    return 0;
}