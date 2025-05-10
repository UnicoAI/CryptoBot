#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <winsock2.h>  // Windows socket library

#pragma comment(lib, "ws2_32.lib")  // Link with the Winsock library

void serveFile(SOCKET clientSocket, const std::string& filePath) {
    // Open the index.html file
    std::ifstream file(filePath);
    if (!file) {
        std::string errorMessage = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\nFile not found!";
        send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
        return;
    }

    // Read the file content into a stringstream
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();

    // Send a basic HTTP header and the content of the file
    std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    send(clientSocket, header.c_str(), header.length(), 0);
    send(clientSocket, fileContent.c_str(), fileContent.length(), 0);
}

void runServer() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return;
    }

    // Create a socket for the server
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return;
    }

    // Define the server address (localhost, port 8080)
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Start listening for incoming connections
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Server is running on http://localhost:8080..." << std::endl;

    // Accept client connections and serve the index.html file
    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed." << std::endl;
            continue;
        }

        // Read the request (we don't process it, just respond with index.html)
        char buffer[1024];
        recv(clientSocket, buffer, sizeof(buffer), 0);

        // Serve the index.html file
        serveFile(clientSocket, "public/index.html");

        // Close the client connection
        closesocket(clientSocket);
    }

    // Close the server socket
    closesocket(serverSocket);
    WSACleanup();
}
