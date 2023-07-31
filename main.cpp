#include <iostream>
#include <vector>
#include <string>
#include <fstream>

double calculate_score(std::string &response, std::string &key);

int main() {

    std::fstream file_stream {"../responses.txt"};
    if(!file_stream) {
        std::cerr << "Cannot locate file!" << std::endl;
        return 1;
    }

    std::string answer_key {};
    std::getline(file_stream, answer_key);

    std::string *student {nullptr};

    while(!file_stream.eof()) {
        if(student == nullptr) {
            student = new std::string;
            std::getline(file_stream, *student);
            continue;
        }

        std::string response {};
        std::getline(file_stream, response);

        std::cout << *student << ": " << calculate_score(response, answer_key) * 100 << "%" << std::endl;

        student = nullptr;
    }

    delete student;
    return 0;
}

double calculate_score(std::string &response, std::string &key) {
    int score {};

    for(int i {}; i < key.length(); i++) if(key[i] == response[i]) score++;

    return score / static_cast<double>(key.length());
}
