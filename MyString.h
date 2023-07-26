#ifndef LEARNINGCPP_MYSTRING_H
#define LEARNINGCPP_MYSTRING_H
#include <iostream>

class MyString {

    friend std::ostream &operator<<(std::ostream &os, const MyString &my_string);
    friend std::istream &operator>>(std::istream &is, MyString &my_string);


private:
    char *str {nullptr};

public:
    explicit MyString(const char *str);
    MyString(const MyString &source);
    MyString(MyString &&source) noexcept;
    [[nodiscard]] char *get_string() const;
    void set_string(const char *str);

    MyString &operator=(const MyString &rhs);
    MyString &operator-();



    ~MyString();
};


#endif
