#include "server.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "game.h"

int server_socket, client_one_socket, client_two_socket;

struct sockaddr_in server_addr_server, client_one_addr, client_two_addr;
socklen_t client_addr_len = sizeof(client_one_addr);

pthread_t starter_thread, server_thread_one, server_thread_two;

void startServer(const int port) {
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

void *distribute_one() {
    while (isGameRunning()) {
        sendByte(2, receiveByte(1));
    }

    return NULL;
}

void *distribute_two() {
    while (isGameRunning()) {
        sendByte(1, receiveByte(2));
    }

    return NULL;
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

    status = STATUS_RUNNING;

    pthread_create(&server_thread_one, NULL, distribute_one, NULL);
    pthread_create(&server_thread_two, NULL, distribute_two, NULL);

    printf("Clients connected! Started server thread %s:%d\n", inet_ntoa(client_two_addr.sin_addr),
           ntohs(client_two_addr.sin_port));

    return NULL;
}

void sendByte(const int id, const char column) {
    if (write(id == 1 ? client_one_socket : client_two_socket, &column, 1) == -1) {
        perror("Error sending data");
    }
}

const char receiveByte(const int id) {
    char temp;
    if (read(id == 1 ? client_one_socket : client_two_socket, &temp, 1) == -1) {
        perror("Error receiving data");
    }
    return temp;
}

void closeServer() {
    closeClients();
    close(server_socket);

    puts("Server closed");
}

void closeClients() {
    pthread_cancel(server_thread_one);
    pthread_join(server_thread_one, NULL);
    close(client_one_socket);

    pthread_cancel(server_thread_two);    
    pthread_join(server_thread_two, NULL);
    close(client_two_socket);

    puts("Clients closed");
}
