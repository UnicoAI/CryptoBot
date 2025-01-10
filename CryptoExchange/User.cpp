#include "User.h"

User::User(const std::string& username, const std::string& password)
    : username(username), password(password) {}

User::~User() {
    for (auto account : accounts) {
        delete account;
    }
}

bool User::authenticate(const std::string& password) const {
    return this->password == password;
}

void User::addAccount(Account* account) {
    accounts.push_back(account);
}

const std::vector<Account*>& User::getAccounts() const {
    return accounts;
}

void User::addNamedAccount(const std::string& name, const std::string& tokenAddress) {
    namedAccounts[name] = tokenAddress;
}

const std::map<std::string, std::string>& User::getNamedAccounts() const {
    return namedAccounts;
}

const std::string& User::getUsername() const {
    return username;
}

