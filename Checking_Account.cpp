#include "Checking_Account.h"
#include "Account.h"

Checking_Account::Checking_Account(std::string name, double balance) : Account(name, balance) {

}

bool Checking_Account::withdraw(double amount) {
    amount += 1.5;
    bool success {Account::withdraw(amount)};

    if (success) std::cout << "Withdrew $" << amount << " from " << name << "'s checking account ($1.50 fee)" << std::endl;
    else std::cout << "Failed to withdraw $" << amount << " from" << name << "'s checking account" << std::endl;

    return success;
}

Account &Checking_Account::operator-=(double amount) {
    withdraw(amount);

    return *this;
}
