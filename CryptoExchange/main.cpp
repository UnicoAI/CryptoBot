#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <json/json.h>
#include "CryptoExchange.h"
#include "EthereumAccount.h"
#include "BitcoinAccount.h"
#include "CryptoBot.h"
#include "DataFetcher.h"

// Function to display user accounts
void displayAccounts(const User* user) {
    int index = 1;
    for (const auto& account : user->getAccounts()) {
        std::cout << index++ << ". ";
        account->displayAccountType();
        std::cout << "Owner: " << account->getOwner() << ", Balance: " << account->getBalance() << std::endl;
    }
}

void displayFetchedData(const std::string& jsonFilePath) {
    try {
        std::ifstream file(jsonFilePath);
        if (file.is_open()) {
            Json::Value jsonData;
            file >> jsonData;

            // Check if jsonData is an object (with coin IDs as keys)
            if (jsonData.isObject()) {
                for (const auto& coinId : jsonData.getMemberNames()) {
                    const Json::Value& coin = jsonData[coinId];

                    // Extract and display coin name
                    if (coin.isMember("name") && coin["name"].isString()) {
                        std::cout << "Coin Name: " << coin["name"].asString() << std::endl;
                    }
                    else {
                        std::cout << "Coin Name: Not available" << std::endl;
                    }

                    // Extract and display price in USD
                    if (coin.isMember("current_price")) {
                        std::cout << "Price (USD): " << coin["current_price"].asDouble() << std::endl;
                    }
                    else {
                        std::cout << "Price (USD): Not available" << std::endl;
                    }

                    // Extract and display 24h price change percentage
                    if (coin.isMember("price_change_percentage_24h")) {
                        std::cout << "24h Price Change (%): " << coin["price_change_percentage_24h"].asDouble() << std::endl;
                    }
                    else {
                        std::cout << "24h Price Change (%): Not available" << std::endl;
                    }

                    // Extract and display circulating supply
                    if (coin.isMember("circulating_supply")) {
                        std::cout << "Circulating Supply: " << coin["circulating_supply"].asDouble() << std::endl;
                    }
                    else {
                        std::cout << "Circulating Supply: Not available" << std::endl;
                    }

                    // Extract and display 24h high and low prices
                    if (coin.isMember("high_24h")) {
                        std::cout << "24h High (USD): " << coin["high_24h"].asDouble() << std::endl;
                    }
                    else {
                        std::cout << "24h High (USD): Not available" << std::endl;
                    }

                    if (coin.isMember("low_24h")) {
                        std::cout << "24h Low (USD): " << coin["low_24h"].asDouble() << std::endl;
                    }
                    else {
                        std::cout << "24h Low (USD): Not available" << std::endl;
                    }

                    std::cout << "-----------------------------" << std::endl; // Separator between coins
                }
            }
            else {
                std::cerr << "Error: JSON data is not an object.\n";
            }

            file.close();
        }
        else {
            std::cerr << "Error: Could not open the JSON file to display fetched data.\n";
        }
    }
    catch (const Json::LogicError& e) {
        std::cerr << "JSON Logic Error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

// Function to start the crypto bot
void startCryptoBot(CryptoBot& bot) {
    bot.start();
}

int main() {
    CryptoExchange exchange;
    CryptoBot cryptoBot(exchange);

    std::cout << "Starting Crypto Bot..." << std::endl;
    std::thread botThread(startCryptoBot, std::ref(cryptoBot));

    // Define the path for the JSON file (only locally within main)
    std::string jsonFilePath = "coin_prices.json"; // Ensure this file exists in the working directory

    // Start the data fetcher for live coin data
    DataFetcher fetcher("127.0.0.1", 5400, jsonFilePath);
    fetcher.start();

    // Register users
    exchange.registerUser("marius", "password123");
    exchange.registerUser("bob", "password456");

    std::string username, password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    // Login the user
    User* user = exchange.loginUser(username, password);
    if (user) {
        std::cout << username << " logged in successfully.\n";

        if (user->getAccounts().empty()) {
            user->addAccount(new EthereumAccount(username, 1000.0));
            user->addAccount(new BitcoinAccount(username, 500.0));
        }

        std::cout << "\nYour accounts:\n";
        displayAccounts(user);

        int choice;
        do {
            std::cout << "\nMenu:\n";
            std::cout << "1. Transfer Money\n";
            std::cout << "2. Crypto Bots\n";
            std::cout << "3. View Fetched Data\n";
            std::cout << "4. Exit\n";
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            switch (choice) {
            case 1: {
                std::string toUsername, tokenAddress;
                int fromAccountIndex;
                double amount;

                std::cout << "Enter the username to transfer to: ";
                std::cin >> toUsername;
                std::cout << "Enter the token address: ";
                std::cin >> tokenAddress;

                std::cout << "Select your account to transfer from:\n";
                displayAccounts(user);
                std::cin >> fromAccountIndex;

                std::cout << "Enter the amount to transfer: ";
                std::cin >> amount;

                if (fromAccountIndex > 0 && fromAccountIndex <= user->getAccounts().size()) {
                    Account* fromAccount = user->getAccounts()[fromAccountIndex - 1];
                    exchange.transferMoneyToAddress(fromAccount, tokenAddress, amount);

                    std::cout << "Transfer successful.\n";
                    std::cout << "Updated balance:\n";
                    std::cout << username << "'s account - Balance: " << fromAccount->getBalance() << std::endl;
                }
                else {
                    std::cout << "Invalid account selection.\n";
                }
                break;
            }
            case 2: {
                int botChoice;
                do {
                    std::cout << "\nCrypto Bot Menu:\n";
                    std::cout << "1. Start Bot\n";
                    std::cout << "2. Stop Bot\n";
                    std::cout << "3. Display Bot Status\n";
                    std::cout << "4. Back to Main Menu\n";
                    std::cout << "Enter your choice: ";
                    std::cin >> botChoice;

                    switch (botChoice) {
                    case 1:
                        if (!cryptoBot.isRunning()) {
                            std::cout << "Starting Crypto Bot...\n";
                            cryptoBot.start();
                        }
                        else {
                            std::cout << "Crypto Bot is already running.\n";
                        }
                        break;
                    case 2:
                        if (cryptoBot.isRunning()) {
                            std::cout << "Stopping Crypto Bot...\n";
                            cryptoBot.stop();
                        }
                        else {
                            std::cout << "Crypto Bot is not running.\n";
                        }
                        break;
                    case 3:
                        std::cout << "\n=== Bot Details ===" << std::endl;
                        cryptoBot.displayBotDetails();  // Call to display detailed bot information
                        break;
                    case 4:
                        std::cout << "Returning to Main Menu...\n";
                        break;
                    default:
                        std::cout << "Invalid choice. Please try again.\n";
                        break;
                    }
                } while (botChoice != 4);
                break;
            }
            case 3:
                // Display fetched coin data
                displayFetchedData(jsonFilePath);
                break;
            case 4:
                std::cout << "Exiting program...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
            }
        } while (choice != 4);
    }
    else {
        std::cout << "Invalid username or password. Exiting...\n";
    }

    // Stop the data fetcher and join threads
    fetcher.stop();
    if (botThread.joinable()) {
        botThread.join();
    }
    return 0;
}
