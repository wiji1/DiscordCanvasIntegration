#ifndef LEARNINGCPP_INSUFFICIENTFUNDSEXCEPTION_H
#define LEARNINGCPP_INSUFFICIENTFUNDSEXCEPTION_H
#include <exception>
#include "AccountException.h"

class InsufficientFundsException : public AccountException {
private:
    double attempted_value;
public:
    explicit InsufficientFundsException(Account &account, double attempted_value)
        : AccountException(account), attempted_value{attempted_value} {
    }

    [[nodiscard]] const char *what() const noexcept override {
        return "Insufficient Funds";
    }

    [[nodiscard]] double get_attempted_value() const {
        return attempted_value;
    }
};


#endif
