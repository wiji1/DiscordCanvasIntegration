#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <stack>

bool is_palindrome(std::string &input);

int main() {
    std::string str {"Drab as a fool, aloof as a basrd."};
    std::cout << std::boolalpha << is_palindrome(str) << std::endl;

    return 0;
}

bool is_palindrome(std::string &input) {
    std::stack<char> stack {};
    std::string compare {};

    for(auto c : input) {
        if(!isalpha(c)) continue;

        compare += static_cast<char>(std::toupper(c));
        stack.push(static_cast<char>(std::toupper(c)));
    }

    int index {};
    while(!stack.empty()) {
        if(stack.top() != compare[index]) return false;
        stack.pop();
        index++;
    }

    return true;
}

