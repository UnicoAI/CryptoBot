#include "CryptoBot.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <winsock2.h>
#include <chrono>

CryptoBot::CryptoBot(CryptoExchange& exchange)
    : running(false), exchange(exchange) {}

CryptoBot::~CryptoBot() {
    stop();
}

void CryptoBot::start() {
    if (running) {
        std::cout << "Bot is already running." << std::endl;
        return;
    }
    running = true;
    std::cout << "CryptoBot is starting..." << std::endl;
    std::thread(&CryptoBot::listenForData, this).detach();
    std::cout << "CryptoBot thread started." << std::endl;
}

void CryptoBot::stop() {
    if (!running) {
        std::cout << "Bot is not running." << std::endl;
        return;
    }
    running = false;
    std::cout << "CryptoBot is stopping..." << std::endl;
}

bool CryptoBot::isRunning() const {
    return running;
}

void CryptoBot::logActivity(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    logData.push_back(message);
    std::cout << "[CryptoBot Log]: " << message << std::endl;
}

void CryptoBot::displayBotDetails() {
    std::lock_guard<std::mutex> lock(logMutex);
    std::cout << "=== CryptoBot Details ===" << std::endl;
    std::cout << "Running Status: " << (running ? "Active" : "Stopped") << std::endl;
    std::cout << "Activity Log:" << std::endl;
    for (const auto& log : logData) {
        std::cout << log << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

nlohmann::json CryptoBot::loadJson(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return nlohmann::json::object();
    nlohmann::json data;
    file >> data;
    return data;
}

void CryptoBot::saveJson(const std::string& filename, const nlohmann::json& data) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << data.dump(4);
    }
}

void CryptoBot::saveTransaction(const std::string& coinId, const std::string& type, double price, double quantity, double profit) {
    nlohmann::json transaction = {
        {"coin_id", coinId},
        {"type", type},
        {"price", price},
        {"quantity", quantity},
        {"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())},
        {"profit", profit}
    };

    // Load existing transactions
    auto transactions = loadJson("bot_coin_transactions.json");

    // Add the new transaction
    transactions.push_back(transaction);

    // Save updated transactions
    saveJson("bot_coin_transactions.json", transactions);
}
void CryptoBot::tradeLogic(const std::string& coinId, double price) {
    logActivity("Entered tradeLogic with coinId: " + coinId + " and price: " + std::to_string(price));

    if (price <= 0.0000001) {
        logActivity("Invalid price for " + coinId + ": " + std::to_string(price) + ". Skipping trade logic.");
        return;
    }

    const std::string portfolioFile = "bot_portfolio.json";
    auto portfolio = loadJson(portfolioFile);

    if (!portfolio.contains(coinId)) {
        portfolio[coinId] = { {"quantity", 0.0}, {"investment", 0.0}, {"last_price", 0.0} };
        logActivity("No existing portfolio for " + coinId + ". Creating new entry.");
    }

    // Log the portfolio state before making any decisions
    logActivity("Portfolio state for " + coinId + ": " + portfolio[coinId].dump());

    double lastPrice = portfolio[coinId].value("last_price", 0.0);
    double quantity = portfolio[coinId]["quantity"];
    double investment = portfolio[coinId]["investment"];
    logActivity("Last price: " + std::to_string(lastPrice) + ", Quantity: " + std::to_string(quantity) + ", Investment: " + std::to_string(investment));

    // BUY logic (check if first time purchase or price drop below 90%)
    if (lastPrice == 0.0 || price < lastPrice * 0.90) {
        logActivity("Triggering BUY logic for " + coinId + " at price: " + std::to_string(price));
        double investmentAmount = 100.0; // USD
        double newQuantity = investmentAmount / price;

        portfolio[coinId]["quantity"] = quantity + newQuantity;
        portfolio[coinId]["investment"] = investment + investmentAmount;
        portfolio[coinId]["last_price"] = price;

        saveJson(portfolioFile, portfolio);

        saveTransaction(coinId, "BUY", price, newQuantity);
        logActivity("Bought " + coinId + ": " + std::to_string(newQuantity) + " at " + std::to_string(price));
    }

    // SELL logic (sell if price is higher than last_price)
    if (quantity > 0 && price > portfolio[coinId]["last_price"].get<double>()) {
        logActivity("Triggering SELL logic for " + coinId + " at price: " + std::to_string(price));
        double sellValue = quantity * price;
        double profit = sellValue - investment;

        portfolio[coinId]["quantity"] = 0.0;
        portfolio[coinId]["investment"] = 0.0;

        saveJson(portfolioFile, portfolio);

        saveTransaction(coinId, "SELL", price, quantity, profit);
        logActivity("Sold " + coinId + ": " + std::to_string(quantity) + " at " + std::to_string(price) + " with profit: " + std::to_string(profit));
    }
}



void CryptoBot::processIncomingData(const std::string& coinData) {
    try {
        std::cout << "[CryptoBot] Received data: " << coinData << std::endl;
        auto jsonData = nlohmann::json::parse(coinData);
        std::string coinId = jsonData.value("id", "Unknown");
        double price = jsonData.value("current_price", 0.0);

        std::cout << "[CryptoBot] Processing data for " << coinId << " with price: " << price << std::endl;

        if (price > 0.0) {
            tradeLogic(coinId, price);
        }
        else {
            logActivity("Invalid or missing price data for " + coinId + ". Skipping trade logic.");
        }
    }
    catch (const std::exception& e) {
        logActivity("Failed to process data: " + std::string(e.what()));
    }
}
void CryptoBot::listenForData() {
    // Initialize Winsock
    WSADATA wsaData;
    int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsResult != 0) {
        std::cerr << "[CryptoBot] WSAStartup failed with error: " << wsResult << std::endl;
        return;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "[CryptoBot] Socket creation failed! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[CryptoBot] Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    if (listen(sock, 1) == SOCKET_ERROR) {
        std::cerr << "[CryptoBot] Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    std::cout << "[CryptoBot] Waiting for connections on port " << SERVER_PORT << "..." << std::endl;

    SOCKET clientSocket = accept(sock, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "[CryptoBot] Accept failed with error: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    std::cout << "[CryptoBot] Connection accepted. Listening for data..." << std::endl;

    char buffer[512];
    int bytesReceived;
    while (running) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::string data(buffer, bytesReceived);
            logActivity("[CryptoBot] Data received from client: " + data);
            processIncomingData(data);  // Process the received data
        }
        else if (bytesReceived == 0) {
            std::cout << "[CryptoBot] Connection closed by client." << std::endl;
            break;
        }
        else {
            std::cerr << "[CryptoBot] recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    closesocket(clientSocket);
    closesocket(sock);
    WSACleanup();
}