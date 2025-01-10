#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "Account.h"

enum class TransactionType { Deposit, Withdraw, Transfer };

class Transaction {
public:
    Transaction(Account* sender, Account* receiver, double amount, TransactionType type);
    void execute();

private:
    Account* sender;
    Account* receiver;
    double amount;
    TransactionType type;
};

#endif // TRANSACTION_H
