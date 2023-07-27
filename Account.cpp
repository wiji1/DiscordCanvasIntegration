#include "Account.h"

#include <utility>
#include "exceptions/IllegalBalanceException.h"
#include "exceptions/InsufficientFundsException.h"

Account::Account(std::string name, double balance) 
    : name{std::move(name)} {

    if(balance < 0) throw IllegalBalanceException(*this);
    this->balance = balance;
}

bool Account::deposit(double amount) {
    if (amount < 0) 
        return false;
    else {
        balance += amount;
        return true;
    }
}

void Account::withdraw(double amount) {

    if (balance-amount >=0) {
        balance-=amount;
    } else throw InsufficientFundsException(*this, amount);
}

double Account::get_balance() const {
    return balance;
}

std::ostream &operator<<(std::ostream &os, const Account &account) {
    os << "[Account: " << account.name << ": " << account.balance << "]";
    return os;
}

Account &Account::operator+=(double amount) {
    deposit(amount);

    return *this;
}

Account &Account::operator-=(double amount) {
    withdraw(amount);

    return *this;
}

Account::~Account() = default;
