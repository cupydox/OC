#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <array>
#include <string>
#include <sstream>
#include "../include/parent.h" 

const char *const INPUT_FILE_NAME = "./input.txt";

std::string ReadFileContent(const std::string& filename) {
    std::ifstream file(filename);
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + '\n';
    }
    return content;
}

void RemoveIfExists(const std::string& filename) {
    if (std::filesystem::exists(filename)) {
        std::filesystem::remove(filename);
    }
}

TEST(ProcessTest, Should_WriteReversedStringsToCorrectFiles) {
    std::array<std::string, 5> data = {
        "hello",
        "thisisaverylongstring",
        "test",
        "anotherteststring",
        "quit"
    };

    std::string expectedResult1 = "olleh\ntset\n"; 
    std::string expectedResult2 = "gnirtsgnolyrevasisiht\ngnirtstsetrehtona\n"; 


    std::stringstream input_stream;
    for (const auto& line : data) {
        input_stream << line << '\n';
    }


    std::string outputFile1 = "output1.txt";
    std::string outputFile2 = "output2.txt";


    char* args[] = {nullptr, const_cast<char*>(outputFile1.c_str()), const_cast<char*>(outputFile2.c_str())};
    Parent(3, args, input_stream);


    std::string result1 = ReadFileContent(outputFile1);
    std::string result2 = ReadFileContent(outputFile2);

    ASSERT_EQ(result1, expectedResult1);
    ASSERT_EQ(result2, expectedResult2);


    RemoveIfExists(outputFile1);
    RemoveIfExists(outputFile2);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
