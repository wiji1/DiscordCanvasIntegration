// Section 15
// Challenge 
#include <iostream>
#include <vector>
#include "Savings_Account.h"
#include "Account_Util.h"
#include "Checking_Account.h"
#include "Trust_Account.h"

using namespace std;

int main() {
    cout.precision(2);
    cout << fixed;

    Savings_Account save {"s", 12};

    // Accounts
    vector<Account *> accounts;
    accounts.emplace_back(new Savings_Account {"Joe", 12});
    accounts.emplace_back(new Trust_Account {"Larry", 674, 0.012});
    accounts.emplace_back(new Checking_Account {"Bob", 1200});
    accounts.emplace_back(new Savings_Account {"Curly", 5000});
    
    display(accounts);
    deposit(accounts, 1000);
    withdraw(accounts,2000);
    
    // Savings 

    vector<Account *> sav_accounts;
//    sav_accounts.emplace_back();
    sav_accounts.emplace_back(new Savings_Account {"Superman"});
    sav_accounts.emplace_back(new Checking_Account {"Batman", 2000});
    sav_accounts.emplace_back(new Trust_Account {"Wonderwoman", 5000, 5.0});

    display(sav_accounts);
    deposit(sav_accounts, 1000);
    withdraw(sav_accounts, 2000);


    Checking_Account checking_account {"Bob", 1250.32};
    checking_account.withdraw(1250);

    Trust_Account trust_account {"Joe", 500, 0.05};
    trust_account.withdraw(100);
    trust_account.withdraw(1);
    trust_account.withdraw(1);
    trust_account.withdraw(95);
    trust_account.deposit(5000);

    cout << "=================================" << endl;
    cout << "PRINT!: ";
    for (const auto &item: accounts) cout << *item << " ";
    cout << endl;

    cout << checking_account.get_balance() << " " << trust_account.get_balance() << endl;

    checking_account += 100;
    trust_account -= 500;

    cout << checking_account.get_balance() << " " << trust_account.get_balance() << endl;


    for(auto &p : accounts) delete p;

    for(auto &p : sav_accounts) delete p;


    return 0;
}

