#include <iostream>
#include <vector>
#include "MyString.h"

void createObjects();

using namespace std;

int main() {
    MyString myString {MyString{"Hello!"}};
    myString *= 3;


    cout << myString * -2 << endl;

    MyString *inputString {nullptr};
    cin >> *inputString;

    cout << "Input: " << inputString << endl;


//    MyString secondString {myString};
//
//
//    secondString.set_string("Goodbye!");
//
//    cout << myString << " " << secondString << endl;
//
//
//    secondString = -secondString;
//    secondString = {MyString{"Hello!"}};
//
////    MyString thirdString {-secondString};
//
//
//    cout << "Equal: " << (myString == secondString) << endl;
//
//    cout << "Strings: " << myString << " " << secondString << " " << endl;
//
//    createObjects();


}

void createObjects() {

}
