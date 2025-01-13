#ifndef WINSOCK_CLIENT_H
#define WINSOCK_CLIENT_H

#include <string>
#include <winsock2.h>

class WinsockClient {
public:
    WinsockClient();
    ~WinsockClient();
    void initialize();
    void sendMessage(const std::string& message);

private:
    SOCKET clientSocket;
    WSADATA wsaData;
    sockaddr_in serverAddr;
};

#endif // WINSOCK_CLIENT_H
