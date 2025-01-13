#ifndef CRYPTOBOT_H
#define CRYPTOBOT_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>  // For JSON parsing and handling
#include <mutex>
#include <thread>
#include <winsock2.h>
#include <chrono>

class CryptoExchange; // Forward declaration to avoid circular dependency

class CryptoBot {
public:
    CryptoBot(CryptoExchange& exchange);
    ~CryptoBot();

    void start();
    void stop();
    bool isRunning() const;
    void displayBotDetails();
    void logActivity(const std::string& message);

    void displayTransactions();  // New method to display all transactions


private:
    void listenForData();
    void processIncomingData(const std::string& coinData);
    void tradeLogic(const std::string& coinId, double price);
    nlohmann::json loadJson(const std::string& filename);
    void saveJson(const std::string& filename, const nlohmann::json& data);
    void saveTransaction(const std::string& coinId, const std::string& type, double price, double quantity, double profit = 0.0);

    bool running;
    CryptoExchange& exchange;
    std::vector<std::string> logData;
    std::mutex logMutex;
    const int SERVER_PORT = 5400;
};

#endif // CRYPTOBOT_H
