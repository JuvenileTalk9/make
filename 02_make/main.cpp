#include <iostream>
#include <string>

int file_check(std::string);

int main(int argc, char** argv) {
    std::string file_path = "main.cpp";
    if (file_check(file_path)) {
        std::cout << file_path << " exists." << std::endl;
    } else {
        std::cout << file_path << " does not exist." << std::endl;
    }
    return 0;
}
