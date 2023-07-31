#include <iostream>
#include <vector>
#include <string>
#include <fstream>

int main() {
    std::string to_lower(std::string &source);

    std::fstream file_stream {"../source.txt"};
    if(!file_stream) {
        std::cerr << "Cannot locate file!" << std::endl;
        return 1;
    }

    std::cout << "Please enter a word you would like to search for: " << std::endl;

    std::string word;
    std::cin >> word;

    int total_word_count {1};
    int selected_word_count {0};

    char current_char;
    std::string current_word;
    bool last_char_space {false};

    while(!file_stream.eof()) {
        file_stream.get(current_char);

        if(current_char == ' ' || current_char == '\n') {
            if(last_char_space) continue;

            last_char_space = true;
            total_word_count++;
            if(to_lower(current_word) == to_lower(word)) selected_word_count++;
            current_word = "";
            continue;
        } else last_char_space = false;

        current_word += current_char;
    }

    std::cout << "Scanning " << total_word_count << " words..." << std::endl;
    std::cout << "The source file has " << selected_word_count << " instances of the word \"" << word << "\"" << std::endl;

    return 0;
}

std::string to_lower(std::string &source) {
    std::string final {};

    for(auto c : source) {
        final += static_cast<char>(tolower(c));
    }

    return final;
}

