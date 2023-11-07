#pragma once

void startServer(const int port);
void *waitForClients();
void *distribute();
void sendByte(const int id, const char column);
const char receiveByte(const int id);
void closeServer();
void closeClients();