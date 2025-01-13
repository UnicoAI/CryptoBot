#ifndef USER_H
#define USER_H

#include <map>
#include <string>
#include <vector>
#include "Account.h"

class User {
    std::string username;
    std::string password;
    std::vector<Account*> accounts;
    std::map<std::string, std::string> namedAccounts; // Map to store named accounts with token addresses

public:
    User(const std::string& username, const std::string& password);
    ~User();
    bool authenticate(const std::string& password) const;
    void addAccount(Account* account);
    const std::vector<Account*>& getAccounts() const;
    void addNamedAccount(const std::string& name, const std::string& tokenAddress);
    const std::map<std::string, std::string>& getNamedAccounts() const;
    const std::string& getUsername() const; // Add this method
};

#endif // USER_H

