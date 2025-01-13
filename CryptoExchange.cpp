#include "CryptoExchange.h"
#include <algorithm>
#include <iostream>

void CryptoExchange::registerUser(const std::string& username, const std::string& password) {
    users.emplace_back(username, password);
}

User* CryptoExchange::loginUser(const std::string& username, const std::string& password) {
    auto it = std::find_if(users.begin(), users.end(), [&username, &password](User& user) {
        return user.getUsername() == username && user.authenticate(password);
        });

    if (it != users.end()) {
        return &(*it);
    }
    return nullptr;
}

void CryptoExchange::transferMoney(Account* fromAccount, Account* toAccount, double amount) {
    if (fromAccount->getBalance() >= amount) {
        fromAccount->withdraw(amount);
        toAccount->deposit(amount);
    }
}

void CryptoExchange::transferMoneyToAddress(Account* fromAccount, const std::string& tokenAddress, double amount) {
    if (fromAccount->getBalance() >= amount) {
        fromAccount->withdraw(amount);

        // Display the account type in the transfer statement
        std::cout << "Transferred " << amount << " from ";
        fromAccount->displayAccountType();
        std::cout << " account to token address: " << tokenAddress << std::endl;
    }
    else {
        std::cout << "Insufficient balance to transfer " << amount << " from ";
        fromAccount->displayAccountType();
        std::cout << " account" << std::endl;
    }
}
