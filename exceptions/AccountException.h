#ifndef LEARNINGCPP_ACCOUNTEXCEPTION_H
#define LEARNINGCPP_ACCOUNTEXCEPTION_H
#include <exception>
#include "../Account.h"


class AccountException : public std::exception {
private:
    Account *account;
public:
    explicit AccountException(Account &account) {
        this->account = &account;
    }

    [[nodiscard]] Account & get_account() const {
        return *account;
    }

    [[nodiscard]] const char *what() const noexcept override = 0;
};


#endif