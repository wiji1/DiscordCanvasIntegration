#ifndef LEARNINGCCP_ACCOUNT_H_
#define LEARNINGCCP_ACCOUNT_H_
#include <iostream>
#include <string>

class Account {
    friend std::ostream &operator<<(std::ostream &os, const Account &account);
private:   
    static constexpr const char *def_name = "Unnamed Account";
    static constexpr double def_balance = 0.0;
protected:
    std::string name;
    double balance;
public:
    explicit Account(std::string name = def_name, double balance = def_balance);

    virtual // Account(std::string name = "Unamed Account", double balance = 0.0);
    bool deposit(double amount);

    virtual bool withdraw(double amount);
    [[nodiscard]] double get_balance() const;

    virtual Account &operator+=(double amount);
    virtual Account &operator-=(double amount);
};
#endif