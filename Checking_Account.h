#ifndef LEARNINGCPP_CHECKING_ACCOUNT_H
#define LEARNINGCPP_CHECKING_ACCOUNT_H

#include <string>
#include "Account.h"

class Checking_Account : public Account {
public:
    Checking_Account(std::string name, double balance);
    bool withdraw(double amount) override;

    Account &operator-=(double amount) override;
};


#endif
