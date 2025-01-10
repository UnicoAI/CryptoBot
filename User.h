#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "Account.h"

class User {
private:
    std::string username;
    std::string password;
    std::vector<Account*> accounts;

public:
    User(const std::string& username, const std::string& password);

    const std::string& getUsername() const;
    bool authenticate(const std::string& password) const;
    void addAccount(Account* account);
    const std::vector<Account*>& getAccounts() const;
};

#endif
