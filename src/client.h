#pragma once

#include <stddef.h>

void connectToServer(const int port, const unsigned long ip);
void sendByteToServer(const char column);
void *receiveBytesFromServer();
void closeClient();
const int hostnameToIp(const char *hostname, char *resolvedIP, size_t resolvedIPSize);