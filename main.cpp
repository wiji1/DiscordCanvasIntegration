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
   
    // Accounts
    vector<Account> accounts;
    accounts.emplace_back();
    accounts.emplace_back("Larry");
    accounts.emplace_back("Moe", 2000);
    accounts.emplace_back("Curly", 5000);
    
    display(accounts);
    deposit(accounts, 1000);
    withdraw(accounts,2000);
    
    // Savings 

    vector<Savings_Account> sav_accounts;
    sav_accounts.emplace_back();
    sav_accounts.emplace_back("Superman");
    sav_accounts.emplace_back("Batman", 2000);
    sav_accounts.emplace_back("Wonderwoman", 5000, 5.0);

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
    cout << checking_account.get_balance() << " " << trust_account.get_balance() << endl;

    checking_account += 100;
    trust_account -= 500;

    cout << checking_account.get_balance() << " " << trust_account.get_balance() << endl;


    return 0;
}

