#ifndef LEARNINGCCP_ACCOUNT_UTIL_H_
#define LEARNINGCCP_ACCOUNT_UTIL_H_
#include <vector>
#include "Account.h"
#include "Savings_Account.h"

void display(const std::vector<Account *> &accounts);
void deposit(std::vector<Account *> &accounts, double amount);
void withdraw(std::vector<Account *> &accounts, double amount);

#endif