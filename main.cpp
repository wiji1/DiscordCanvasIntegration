#include <iostream>
#include <vector>
#include "MyString.h"

void createObjects();

using namespace std;

int main() {
    MyString myString {"Hellso!"};
    MyString secondString {myString};


    secondString.set_string("Goodbye!");

    cout << myString << " " << secondString << endl;


    secondString = -secondString;

//    MyString thirdString {-secondString};

    cout << myString << " " << secondString << " " << endl;

    createObjects();


}

void createObjects() {

}
