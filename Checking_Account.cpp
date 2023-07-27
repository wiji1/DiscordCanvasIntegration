#include "Checking_Account.h"
#include "Account.h"

#include <utility>

Checking_Account::Checking_Account(std::string name, double balance) : Account(std::move(name), balance) {

}

void Checking_Account::withdraw(double amount) {
    amount += 1.5;
    Account::withdraw(amount);

    std::cout << "Withdrew $" << amount << " from " << name << "'s checking account ($1.50 fee)" << std::endl;
}

bool Checking_Account::deposit(double amount) {
    return Account::deposit(amount);
}

Account &Checking_Account::operator-=(double amount) {
    withdraw(amount);

    return *this;
}

void Checking_Account::print(std::ostream &os) {
    os << "Checking Account";
}

