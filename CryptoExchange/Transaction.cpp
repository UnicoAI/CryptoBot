#include "Transaction.h"
#include "Account.h"
#include <iostream>

void processTransaction(const Account& source, const Account& destination, double amount) {
    std::cout << "Transaction initiated from " << source.getOwner()
        << " to " << destination.getOwner() << " for amount $" << amount << ".\n";

    // Additional transaction logic...
}
