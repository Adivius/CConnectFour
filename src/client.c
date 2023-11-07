#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "game.h"
#include "consts.h"

int client_socket;

struct sockaddr_in server_addr_client;
socklen_t server_addr_client_length = sizeof(server_addr_client);

pthread_t listen_thread;

void connectToServer(int port, char *ip) {

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // Configure server address
    server_addr_client.sin_family = AF_INET;
    server_addr_client.sin_addr.s_addr = inet_addr(ip);
    server_addr_client.sin_port = htons(port);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *) &server_addr_client, sizeof(server_addr_client)) == -1) {
        perror("Error connecting to server");
        exit(1);
    }

    printf("Connected to server %s:%d\n", ip, port);

    pthread_create(&listen_thread, NULL, receiveBytesFromServer, NULL);

    printf("Started listen thread\n");
}

void sendByteToServer(char column) {
    if (write(client_socket, &column, 1) == -1) {
        perror("Error sending data");
    }
}

void *receiveBytesFromServer() {
    while (1) {
        char temp;
        if (read(client_socket, &temp, 1) == -1) {
            perror("Error receiving data");
            exit(1);
        }

        int column = (int) temp;

        if (column == PACKAGE_REMATCH) {
            reset();
        } else {
            update(column);
        }
    }

    return NULL;

}

void closeClient() {
    pthread_cancel(listen_thread);
    close(client_socket);
}


int hostnameToIp(const char *hostname, char *resolvedIP, size_t resolvedIPSize) {
    struct addrinfo *info;

    // Resolve the hostname
    if (getaddrinfo(hostname, NULL, NULL, &info) != 0) {
        return -1; // Failed to resolve hostname
    }

    // Convert to string
    if (inet_ntop(AF_INET, &(((struct sockaddr_in *)(info->ai_addr))->sin_addr), resolvedIP, resolvedIPSize) == NULL) {
        freeaddrinfo(info);
        return -1; // Failed to convert to IP string
    }

    freeaddrinfo(info); // Free the memory allocated by getaddrinfo
    return 0; // Success
}