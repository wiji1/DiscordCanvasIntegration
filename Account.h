#ifndef LEARNINGCCP_ACCOUNT_H_
#define LEARNINGCCP_ACCOUNT_H_
#include <iostream>
#include <string>
#include "I_Printable.h"

class Account : public I_Printable {
    friend std::ostream &operator<<(std::ostream &os, const Account &account);
private:   
    static constexpr const char *def_name = "Unnamed Account";
    static constexpr double def_balance = 0.0;
protected:
    std::string name;
    double balance;
public:
    explicit Account(std::string name = def_name, double balance = def_balance);
    virtual ~Account();

    virtual bool deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0;

    std::string get_name() {
        return name;
    }

    [[nodiscard]] double get_balance() const;

    virtual Account &operator+=(double amount);
    virtual Account &operator-=(double amount);

    void print(std::ostream &os) override = 0;

};
#endif