#ifndef BITCOINACCOUNT_H
#define BITCOINACCOUNT_H

#include "Account.h"

class BitcoinAccount : public Account {
public:
    BitcoinAccount(const std::string& accountOwner, double initialBalance = 0.0)
        : Account(accountOwner, initialBalance) {}

    void displayAccountType() const override;
};

#endif
