#include "Account.h"

Account::Account(const std::string& accountOwner, double initialBalance) 
    : owner(accountOwner), balance(initialBalance) {}

const std::string& Account::getOwner() const {
    return owner;
}

double Account::getBalance() const {
    return balance;
}

void Account::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
    }
}

void Account::withdraw(double amount) {
    if (amount > 0 && amount <= balance) {
        balance -= amount;
    }
}
