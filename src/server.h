#pragma once

void startServer(const int port);

void *waitForClients();

void *distribute();

const char receiveByte(const int id);

void sendByte(const int id, const char column);

void closeClients();

void closeServer();