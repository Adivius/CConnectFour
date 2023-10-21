#pragma once

void startServer(int port);
void *waitForClients();
void *distribute();
void sendByte(int id, char column);
char receiveByte(int id);
void closeServer();
void closeClients();

extern int status;