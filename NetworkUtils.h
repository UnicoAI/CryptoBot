#pragma once
#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <string>
#include <winsock2.h>

void initWinsock();
SOCKET createSocket(const std::string& ip, int port);
void sendCoinData(SOCKET sock, const std::string& coinData);

#endif
