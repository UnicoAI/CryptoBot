#include "WinsockClient.h"
#include <iostream>
#include <ws2tcpip.h>

WinsockClient::WinsockClient() : clientSocket(INVALID_SOCKET), wsaData{}, serverAddr{} {}

WinsockClient::~WinsockClient() {
    closesocket(clientSocket);
    WSACleanup();
}

void WinsockClient::initialize() {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        exit(1);
    }
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed.\n";
        exit(1);
    }
}

void WinsockClient::sendMessage(const std::string& message) {
    send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0);

    std::cout << "Message sent: " << message << "\n";
}
