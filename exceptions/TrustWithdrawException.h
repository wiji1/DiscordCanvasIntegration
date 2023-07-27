#ifndef LEARNINGCPP_TRUSTWITHDRAWEXCEPTION_H
#define LEARNINGCPP_TRUSTWITHDRAWEXCEPTION_H
#include "AccountException.h"

class TrustWithdrawException : public AccountException {
private:
    bool limit;
public:
    explicit TrustWithdrawException(Account &account, bool limit) : AccountException(account), limit{limit} {

    }

    [[nodiscard]] const char *what() const noexcept override {
        if (limit) return "Maximum yearly withdraws reached";
        else return "Withdraw amount must be less than 20% of balance";
    }


    [[nodiscard]] bool is_limit() const {
        return limit;
    }
};


#endif
