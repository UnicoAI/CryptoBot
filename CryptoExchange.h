#ifndef CRYPTOEXCHANGE_H
#define CRYPTOEXCHANGE_H

#include <vector>
#include "User.h"

class CryptoExchange {
private:
    std::vector<User> users;

public:
    void registerUser(const std::string& username, const std::string& password);
    User* loginUser(const std::string& username, const std::string& password);
    void transferMoney(Account* fromAccount, Account* toAccount, double amount);
};

#endif
