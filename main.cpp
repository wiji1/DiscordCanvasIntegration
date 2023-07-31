#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

int main() {
    bool is_empty(std::string &source);

    std::fstream input_stream {"../source.txt"};
    if(!input_stream) {
        std::cerr << "Cannot locate file!" << std::endl;
        return 1;
    }

    std::ofstream output_stream {"../output.txt"};

    int line_num {1};
    std::string current_line {};

    while(!input_stream.eof()) {
        std::getline(input_stream, current_line);
        if(!is_empty(current_line)) output_stream << line_num << "     ";
        output_stream <<  current_line << std::endl;

        line_num++;
    }

    input_stream.close();
    output_stream.close();
    return 0;
}

bool is_empty(std::string &source) {
    std::string final {};

    for(auto c : source) {
        if(c != ' ' && c != '\n') return false;
    }

    return true;
}

