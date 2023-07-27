#ifndef LEARNINGCPP_ILLEGALBALANCEEXCEPTION_H
#define LEARNINGCPP_ILLEGALBALANCEEXCEPTION_H
#include "AccountException.h"
#include "../Account.h"

class IllegalBalanceException : public AccountException {
public:

    explicit IllegalBalanceException(Account &account) : AccountException(account) {

    }

    [[nodiscard]] const char *what() const noexcept override {
        return "Illegal Account Balance";
    }
};


#endif
