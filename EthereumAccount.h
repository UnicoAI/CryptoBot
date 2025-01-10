#ifndef ETHEREUMACCOUNT_H
#define ETHEREUMACCOUNT_H

#include "Account.h"

class EthereumAccount : public Account {
public:
    EthereumAccount(const std::string& accountOwner, double initialBalance = 0.0)
        : Account(accountOwner, initialBalance) {}

    void displayAccountType() const override;
};

#endif
