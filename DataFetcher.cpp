#include "DataFetcher.h"
#include <stdexcept>
#include <winsock2.h>
#include <curl/curl.h>
#include <ws2tcpip.h>
#include <json/json.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

DataFetcher::DataFetcher(const std::string& serverIp, int serverPort, const std::string& jsonFilePath)
    : ip(serverIp), port(serverPort), running(false), sock(INVALID_SOCKET), jsonFilePath(jsonFilePath) {}

DataFetcher::~DataFetcher() {
    stop();
}

void DataFetcher::start() {
    running = true;
    initWinsock();
    sock = createSocket(ip, port);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    fetcherThread = std::thread([this]() {
        while (running) {
            try {
                // Fetch data for each coin
                updateCoinData("bitcoin");
                updateCoinData("ethereum");
                updateCoinData("dogecoin");

                // Delay the next fetch (e.g., every 60 seconds)
                std::this_thread::sleep_for(std::chrono::minutes(1));
            }
            catch (const std::exception& ex) {
                std::cerr << "[DataFetcher] Error: " << ex.what() << "\nRetrying in 1 minute...\n";
                std::this_thread::sleep_for(std::chrono::minutes(1));
            }
        }
        });
}

void DataFetcher::stop() {
    running = false;
    if (fetcherThread.joinable()) {
        fetcherThread.join();
    }
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }
    WSACleanup();
    curl_global_cleanup();
}

void DataFetcher::initWinsock() {
    WSADATA wsaData;
    int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsResult != 0) {
        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(wsResult));
    }
}

SOCKET DataFetcher::createSocket(const std::string& ip, int port) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        throw std::runtime_error("Socket creation failed with error: " + std::to_string(WSAGetLastError()));
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(sock);
        throw std::runtime_error("Socket connection failed with error: " + std::to_string(WSAGetLastError()));
    }

    return sock;
}

std::string DataFetcher::fetchLiveCoinData(const std::string& coinId) {
    CURL* curl = curl_easy_init();
    std::string data;

    if (curl) {
        std::string url = "https://api.coingecko.com/api/v3/coins/" + coinId;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL failed for " << coinId << ": " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }

    return data;
}

void DataFetcher::updateCoinData(const std::string& coinId) {
    std::string rawData = fetchLiveCoinData(coinId);
    if (rawData.empty()) {
        std::cerr << "[DataFetcher] No data received for " << coinId << "\n";
        return;
    }

    Json::Value jsonData;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream ss(rawData);

    if (Json::parseFromStream(reader, ss, &jsonData, &errs)) {
        Json::Value newData;
        newData["id"] = coinId;

        // Only add values if they exist and are not null
        if (!jsonData["name"].isNull()) {
            newData["name"] = jsonData["name"];
        }
        else {
            newData["name"] = "Unknown";
        }

        if (!jsonData["symbol"].isNull()) {
            newData["symbol"] = jsonData["symbol"];
        }
        else {
            newData["symbol"] = "Unknown";
        }

        if (!jsonData["market_data"]["current_price"]["usd"].isNull()) {
            newData["current_price"] = jsonData["market_data"]["current_price"]["usd"];
        }
        else {
            newData["current_price"] = 0.0;
        }

        // Save to JSON file
        saveToJSON(newData, coinId);

        // Send the data to the bot
        sendDataToBot(newData);
    }
    else {
        std::cerr << "[DataFetcher] JSON parsing error for " << coinId << ": " << errs << "\n";
    }
}

void DataFetcher::saveToJSON(const Json::Value& coinData, const std::string& coinId) {
    Json::Value root;

    // Load existing JSON file if it exists
    std::ifstream inFile(jsonFilePath, std::ifstream::binary);
    if (inFile.is_open()) {
        inFile >> root;
        inFile.close();
    }

    // Ensure root is an object
    if (!root.isObject()) {
        root = Json::Value(Json::objectValue);
    }

    // Update or add new data for the coin
    root[coinId] = coinData;

    // Save updated JSON data back to file
    std::ofstream outFile(jsonFilePath, std::ofstream::binary);
    if (outFile.is_open()) {
        outFile << root;
        outFile.close();
        std::cout << "[DataFetcher] Data for " << coinId << " saved to " << jsonFilePath << "\n";
    }
    else {
        std::cerr << "[DataFetcher] Failed to open file " << jsonFilePath << " for writing.\n";
    }
}

void DataFetcher::sendDataToBot(const Json::Value& coinData) {
    // Convert the coin data to a string
    Json::StreamWriterBuilder writer;
    std::string dataToSend = Json::writeString(writer, coinData);

    if (sock == INVALID_SOCKET) {
        std::cerr << "[DataFetcher] Socket is invalid. Data cannot be sent." << std::endl;
        throw std::runtime_error("Invalid socket");
    }

    // Send the data to the bot
    int bytesSent = send(sock, dataToSend.c_str(), static_cast<int>(dataToSend.size()), 0);

    if (bytesSent == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();
        std::cerr << "[DataFetcher] Failed to send data to bot. Error code: " << errorCode << "\n";
        // Additional check for socket errors
        if (errorCode == WSAECONNRESET) {
            std::cerr << "[DataFetcher] Connection reset by peer." << std::endl;
        }
        else if (errorCode == WSAENOTSOCK) {
            std::cerr << "[DataFetcher] Invalid socket." << std::endl;
        }
        else {
            std::cerr << "[DataFetcher] Unknown socket error occurred." << std::endl;
        }
        throw std::system_error(errorCode, std::system_category(), "Socket send error");
    }
    else if (bytesSent < static_cast<int>(dataToSend.size())) {
        std::cerr << "[DataFetcher] Partial data sent. Only " << bytesSent << " bytes of " << dataToSend.size() << " bytes were sent." << std::endl;
    }
    else {
        std::cout << "[DataFetcher] Data sent to bot successfully: " << bytesSent << " bytes." << std::endl;
    }
}

size_t DataFetcher::WriteCallback(void* contents, size_t size, size_t nmemb, void* output) {
    size_t totalSize = size * nmemb;
    std::string* outputStr = (std::string*)output;
    outputStr->append((char*)contents, totalSize);
    return totalSize;
}
