#include "server.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

int server_socket, client_one_socket, client_two_socket;

struct sockaddr_in server_addr_server, client_one_addr, client_two_addr;
socklen_t client_addr_len = sizeof(client_one_addr);

int player = 1;

pthread_t starter_thread;
pthread_t server_thread;


void startServer(int port) {

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // Configure server address
    server_addr_server.sin_family = AF_INET;
    server_addr_server.sin_addr.s_addr = INADDR_ANY;
    server_addr_server.sin_port = htons(port);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *) &server_addr_server, sizeof(server_addr_server)) == -1) {
        perror("Error binding socket");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 2) == -1) {
        perror("Error listening for connections");
        exit(1);
    }

    printf("Server listening on port %d\n", port);
    pthread_create(&starter_thread, NULL, waitForClients, NULL);

}

void *waitForClients() {

    client_one_socket = accept(server_socket, (struct sockaddr *) &client_one_addr, &client_addr_len);
    if (client_one_socket == -1) {
        perror("Error accepting connection");
        exit(1);
    }

    client_two_socket = accept(server_socket, (struct sockaddr *) &client_two_addr, &client_addr_len);
    if (client_two_socket == -1) {
        perror("Error accepting connection");
        exit(1);
    }

    status = 2;

    pthread_create(&server_thread, NULL, distribute, NULL);

    printf("Clients connected! Started server thread %s:%d\n", inet_ntoa(client_two_addr.sin_addr),
           ntohs(client_two_addr.sin_port));


    return NULL;

}


void *distribute() {
    while (status == 2) {
        char column = receiveByte(player);
        player = player == 1 ? 2 : 1;
        sendByte(player, column);
    }

    return NULL;
}

void sendByte(int id, char column) {
    if (write(id == 1 ? client_one_socket : client_two_socket, &column, 1) == -1) {
        perror("Error sending data");
        exit(1);
    }
}


char receiveByte(int id) {
    char temp;
    if (read(id == 1 ? client_one_socket : client_two_socket, &temp, 1) == -1) {
        perror("Error receiving data");
        exit(1);
    }
    printf("CLIENT SAYS %c\n", temp);
    return temp;
}

void closeServer() {
    close(server_socket);
}

void closeClients() {
    close(client_one_socket);
    close(client_two_socket);
}