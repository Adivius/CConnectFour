#include <stddef.h>

void connectToServer(int port, char *ip);
void sendByteToServer(char column);
void *receiveBytesFromServer();
void closeClient();
int hostnameToIp(const char *hostname, char *resolvedIP, size_t resolvedIPSize);