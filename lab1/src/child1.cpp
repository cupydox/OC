#include <iostream>
#include <unistd.h>
#include <string>
#include <algorithm>


int main(int args, char* argv[]){
    if (args < 3){
        std::cerr << "Not provided" << std::endl;
        return 1;
    }

    int pipe_fd = std::stoi(argv[1]);

    FILE *file = fopen(argv[2], "w");
    if (!file) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    char buffer[1024];
    ssize_t bytesRead;

    while ((bytesRead = read(pipe_fd, buffer, sizeof(buffer) - 1)) > 0){
        buffer[bytesRead] = '\0';
        std::string data(buffer);

        size_t pos;
        while((pos = data.find('\n')) != std::string::npos){
            std::string line = data.substr(0, pos);
            std::reverse(line.begin(), line.end());

            line += '\n';
            fwrite(line.c_str(), sizeof(char), line.length(), file);

            data.erase(0, pos + 1);
        }
    }

    fclose(file);
    close(pipe_fd);

    return 0;
}

