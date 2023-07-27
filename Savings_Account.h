#ifndef LEARNINGCCP_SAVINGS_ACCOUNT_H_
#define LEARNINGCCP_SAVINGS_ACCOUNT_H_
#include "Account.h"

class Savings_Account: public Account {
    friend std::ostream &operator<<(std::ostream &os, const Savings_Account &account);
private:
    static constexpr const char *def_name = "Unnamed Savings Account";
    static constexpr double def_balance = 0.0;
    static constexpr double def_int_rate = 0.0;
protected:
    double int_rate;
public:
    explicit Savings_Account(std::string name = def_name, double balance =def_balance, double int_rate = def_int_rate);
    bool deposit(double amount) override;
    void withdraw(double amount) override;

    Account &operator+=(double amount) override;

    void print(std::ostream &os) override;
};

#endif
