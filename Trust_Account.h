#ifndef LEARNINGCPP_TRUST_ACCOUNT_H
#define LEARNINGCPP_TRUST_ACCOUNT_H

#include "Savings_Account.h"
#include <string>


class Trust_Account : public Savings_Account {
public:
    Trust_Account(std::string name, double balance, double interest_rate);
    bool deposit(double amount) override;
    bool withdraw(double amount) override;

    Account &operator+=(double amount) override;
    Account &operator-=(double amount) override;

    void print(std::ostream &os) override;
};


#endif
