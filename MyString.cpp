#include "MyString.h"
#include <iostream>
#include "cstring"

MyString::MyString(const char *str) {
    this->str = new char[std::strlen(str) + 1];
    std::strcpy(this->str, str);
//    std::cout << "Using regular constructor: " << str << std::endl;
}

char *MyString::get_string() const {
    return str;
}

MyString::MyString(const MyString &source) {
    this->str = new char[std::strlen((source.str)) + 1];
    std::strcpy(this->str, source.str);

//    std::cout << "Using copy constructor: " << source.str << std::endl;
}

MyString::MyString(MyString &&source) noexcept {
    this->str = new char[std::strlen(source.str) + 1];
    *(this->str) = *(source.str);
//    std::cout << "Using move constructor: " << source.str << std::endl;
    source.str = nullptr;

}

MyString::~MyString() {
    if(str != nullptr) std::cout << "Using destructor: " << str << std::endl;
//    else std::cout << "Using destructor" << std::endl;

    delete [] str;
}

void MyString::set_string(const char *str) {
    std::strcpy(this->str, str);
}

std::ostream &operator<<(std::ostream &os, const MyString &my_string) {
    os << (my_string.str);
    return os;
}

std::istream &operator>>(std::istream &is, MyString &my_string) {
    char *temp = new char[1000];
    is >> temp;

    my_string.set_string(temp);
    return is;
}

MyString &MyString::operator-() {
    char *buff = new char[std::strlen(str) + 1];
    std::strcpy(buff, str);

    for(int i{}; i < std::strlen(buff); i++) {
        buff[i] = std::tolower(buff[i]);
    }

    auto *temp = new MyString(buff);

    delete [] buff;
    return *temp;
}

MyString &MyString::operator=(const MyString &rhs) {
    if(this == &rhs) return *this;
    std::cout << "Before: " << *rhs.get_string() << std::endl;

    this->str = new char[std::strlen((rhs.str)) + 1];
    std::cout << *rhs.str << std::endl;
    std::strcpy(this->str,  rhs.str);

    return *this;
}
