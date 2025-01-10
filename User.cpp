#include "User.h"

User::User(const std::string& username, const std::string& password)
    : username(username), password(password) {}

const std::string& User::getUsername() const {
    return username;
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
