#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

class Account {
private:
    std::string owner; // Account owner's name
    double balance;    // Account balance

public:
    // Constructor
    Account(const std::string& accountOwner, double initialBalance = 0.0);

    virtual ~Account() = default; // Virtual destructor for proper cleanup of derived classes

    // Pure virtual method for displaying the account type (must be implemented in derived classes)
    virtual void displayAccountType() const = 0;

    // Getters
    const std::string& getOwner() const;
    double getBalance() const;

    // Account operations
    void deposit(double amount);
    void withdraw(double amount);
};

#endif
