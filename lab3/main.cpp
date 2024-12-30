#include <unistd.h> 
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <cstring> 
#include "include/parent.h"

int main(int argc, char* argv[]) {
    return Parent(argc, argv, std::cin);
}