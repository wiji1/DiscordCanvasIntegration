#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <deque>

bool is_palindrome(std::string &input);

int main() {
    std::string str {"Absa"};
    std::cout << std::boolalpha << is_palindrome(str) << std::endl;

    return 0;
}

bool is_palindrome(std::string &input) {
    std::deque<char> deque {};
    for(auto c : input) {
        if(!isalnum(c)) continue;
        deque.push_front(static_cast<char>(toupper(c)));
    }

    while(deque.size() > 1) {
        char f {deque.front()};
        char b {deque.back()};

        if(f != b) return false;
        deque.pop_front();
        deque.pop_back();
    }

    return true;
}

