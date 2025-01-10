#include "CryptoBotServer.h"
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <winsock2.h>

CryptoBotServer::CryptoBotServer() : running(false), serverSocket(INVALID_SOCKET), clientSocket(INVALID_SOCKET) {}

CryptoBotServer::~CryptoBotServer() {
    stop();
}

void CryptoBotServer::start() {
    initWinsock();

    serverSocket = createServerSocket();
    std::cout << "Server listening on port " << SERVER_PORT << "..." << std::endl;

    clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        throw std::runtime_error("Accept failed!");
    }

    running = true;
    serverThread = std::thread(&CryptoBotServer::run, this);
}

void CryptoBotServer::stop() {
    running = false;
    if (serverThread.joinable()) {
        serverThread.join();
    }
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
    }
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
    }
    WSACleanup();
}

void CryptoBotServer::initWinsock() {
    WSADATA wsaData;
    int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsResult != 0) {
        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(wsResult));
    }
}

SOCKET CryptoBotServer::createServerSocket() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        throw std::runtime_error("Socket creation failed!");
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();  // Get the error code
        std::string errorMessage = "Bind failed with error code: " + std::to_string(errorCode);
        closesocket(sock);
        throw std::runtime_error(errorMessage);  // Throw the error with the code
    }

    if (listen(sock, 1) == SOCKET_ERROR) {
        closesocket(sock);
        throw std::runtime_error("Listen failed!");
    }

    return sock;
}

void CryptoBotServer::sendCoinData(const std::string& data) {
    send(clientSocket, data.c_str(), static_cast<int>(data.size()), 0);
}

void CryptoBotServer::run() {
    try {
        // Simulate sending initial coin data
        std::string coinData = "bitcoin:45000";
        sendCoinData(coinData);

        // Keep sending updates every 30 seconds (simulate)
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            coinData = "ethereum:3500";  // Example updated price
            sendCoinData(coinData);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error in server loop: " << e.what() << std::endl;
    }
}
