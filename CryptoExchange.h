#ifndef CRYPTOEXCHANGE_H
#define CRYPTOEXCHANGE_H

#include <vector>
#include <string>
#include "User.h"
#include "Account.h"

class CryptoExchange {
    std::vector<User> users;

public:
    void registerUser(const std::string& username, const std::string& password);
    User* loginUser(const std::string& username, const std::string& password);
    void transferMoney(Account* fromAccount, Account* toAccount, double amount);
    void transferMoneyToAddress(Account* fromAccount, const std::string& tokenAddress, double amount);
};

#endif // CRYPTOEXCHANGE_H

