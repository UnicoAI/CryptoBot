#pragma once
#ifndef CUSTOM_EXCEPTIONS_H
#define CUSTOM_EXCEPTIONS_H

#include <exception>
#include <string>

// Base class for custom exceptions
class CryptoException : public std::exception {
protected:
    std::string message;
public:
    explicit CryptoException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// File-related exception
class FileNotFoundException : public CryptoException {
public:
    explicit FileNotFoundException(const std::string& filename)
        : CryptoException("File not found: " + filename) {}
};

// JSON-related exception
class JsonParsingException : public CryptoException {
public:
    explicit JsonParsingException(const std::string& details)
        : CryptoException("JSON Parsing Error: " + details) {}
};

// Invalid account exception
class InvalidAccountException : public CryptoException {
public:
    explicit InvalidAccountException(const std::string& details)
        : CryptoException("Invalid Account Error: " + details) {}
};

// Insufficient funds exception
class InsufficientFundsException : public CryptoException {
public:
    explicit InsufficientFundsException(double amount)
        : CryptoException("Insufficient funds for the transfer. Requested amount: " + std::to_string(amount)) {}
};

#endif
