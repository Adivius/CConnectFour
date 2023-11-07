#pragma once

const int resolveHostnameToIp(const char *hostname, char *resolvedIP);
void connectToServer(const int port, const unsigned long ip);
void sendByteToServer(const char column);
void *receiveBytesFromServer();
void closeClient();