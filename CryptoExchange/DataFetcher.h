#ifndef DATAFETCHER_H
#define DATAFETCHER_H

#include <string>
#include <winsock2.h>
#include <curl/curl.h>
#include <thread>
#include <json/json.h>

class DataFetcher {
public:
    DataFetcher(const std::string& serverIp, int serverPort, const std::string& jsonFilePath);
    ~DataFetcher();

    void start();
    void stop();

private:
    void initWinsock();
    SOCKET createSocket(const std::string& ip, int port);
    std::string fetchLiveCoinData(const std::string& coinId);
    void updateCoinData(const std::string& coinId);
    void saveToJSON(const Json::Value& coinData, const std::string& coinId);
    void sendDataToBot(const Json::Value& coinData);

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* output);

    std::string ip;
    int port;
    bool running;
    SOCKET sock;
    std::string jsonFilePath;
    std::thread fetcherThread;
};

#endif // DATAFETCHER_H
