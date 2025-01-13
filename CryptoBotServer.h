#ifndef CRYPTO_BOT_SERVER_H
#define CRYPTO_BOT_SERVER_H

#include <string>
#include <atomic>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>  // For inet_pton

#define SERVER_PORT 54000

class CryptoBotServer {
public:
    CryptoBotServer();
    ~CryptoBotServer();

    void start();
    void stop();

private:
    std::atomic<bool> running;
    SOCKET serverSocket;
    SOCKET clientSocket;
    std::thread serverThread;

    void initWinsock();
    SOCKET createServerSocket();
    void sendCoinData(const std::string& data);
    void run();
};

#endif // CRYPTO_BOT_SERVER_H
#pragma once
