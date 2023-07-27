#ifndef LEARNINGCPP_I_PRINTABLE_H
#define LEARNINGCPP_I_PRINTABLE_H

#include <iostream>


class I_Printable {
    friend std::ostream &operator<<(std::ostream &os, I_Printable &rhs);
public:
    virtual void print(std::ostream &os) = 0;
};


#endif
