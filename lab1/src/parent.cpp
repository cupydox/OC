#include <unistd.h> 
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <cstring> 


int Parent(int argc, char* argv[], std::istream& in) {
    if (argc < 3) {
        std::cerr << "Enter the file names correctly" << std::endl;
        return 1;
    }

    

    std::string filename1 = argv[1];
    std::string filename2 = argv[2];

    int pipefd1[2], pipefd2[2];

    if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 == -1){
        perror("fork");
        return 1;
    }

    if(pid1 == 0){
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);  

        execl("./child1", "child1", std::to_string(pipefd1[0]).c_str(), filename1.c_str(), nullptr);
        perror("execl");
        return 1;
    } else {

        pid_t pid2 = fork();
        if (pid2 == -1){
            perror("fork");
            return 1;
        }

        if(pid2 == 0){
            close(pipefd2[1]);
            close(pipefd1[0]);
            close(pipefd1[1]);

            execl("./child2", "child2", std::to_string(pipefd2[0]).c_str(), filename2.c_str(), nullptr);
            perror("execl");
            return 1;
        } else {
        
            close(pipefd1[0]);
            close(pipefd2[0]);

            std::string input;
            std::cout << "Enter strings (enter 'quit' to quit)" << std::endl;


            while(true) {
                std::getline(in, input);
                if (input == "quit") break;

                if (input.length() > 10) {
                    input += '\n';
                    write(pipefd2[1], input.c_str(), input.size());
                    std::cout << "Written to child2: " << input << std::endl;
                } else {
                    input += '\n';
                    write(pipefd1[1], input.c_str(), input.size());
                    std::cout << "Written to child1: " << input << std::endl;
                }
            }

            close(pipefd1[1]);
            close(pipefd2[1]);

            int status;
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);
        }
    }
    return 0;
}