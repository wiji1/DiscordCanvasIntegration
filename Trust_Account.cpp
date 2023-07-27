#include "Trust_Account.h"

#include <utility>
#include "exceptions/TrustWithdrawException.h"


Trust_Account::Trust_Account(std::string name, double balance, double interest_rate)
    : Savings_Account(std::move(name), balance,interest_rate) {

}

bool Trust_Account::deposit(double amount) {
    if (amount >= 5000) {
        amount += 50;

        std::cout << "A bonus of $50 was added to " << name << "'s account for making a deposit of $5000 or more!" << std::endl;
    }

    return Savings_Account::deposit(amount);
}

void Trust_Account::withdraw(double amount) {
    static int withdraws {0};

    if (withdraws > 3) throw TrustWithdrawException(*this, true);
    if (amount >= 0.2 * balance) throw TrustWithdrawException(*this, false);

    Savings_Account::withdraw(amount);
    withdraws++;
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
