#include "Trust_Account.h"


Trust_Account::Trust_Account(std::string name, double balance, double interest_rate)
    : Savings_Account(name, balance,interest_rate) {

}

bool Trust_Account::deposit(double amount) {
    if (amount >= 5000) {
        amount += 50;

        std::cout << "A bonus of $50 was added to " << name << "'s account for making a deposit of $5000 or more!" << std::endl;
    }

    return Savings_Account::deposit(amount);
}

bool Trust_Account::withdraw(double amount) {
    static int withdraws {0};

    if (withdraws > 3) {
        std::cout << "Failed to withdraw money into " << name << "'s account. There have been too many withdraws this year." << std::endl;
        return false;
    }

    if (amount >= 0.2 * balance) {
        std::cout << "Failed to withdraw money into " << name << "'s account. Amount must be less than 20% of balance!" << std::endl;
        return false;
    }

    bool success {Account::withdraw(amount)};
    if (success) withdraws++;
    return success;
}

Account &Trust_Account::operator+=(double amount) {
    deposit(amount);

    return *this;
}

Account &Trust_Account::operator-=(double amount) {
    withdraw(amount);

    return *this;
}

void Trust_Account::print(std::ostream &os) {
    os << "Trust Account";
}
