#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 32

int server_socket, client_socket;

struct sockaddr_in server_addr, client_addr;
socklen_t client_addr_len = sizeof(client_addr);
char buffer[BUFFER_SIZE];

void *waitForClient(int *status) {

    client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_len);
    if (client_socket == -1) {
        perror("Error accepting connection");
        return NULL;
    }

    printf("Client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    *status = 2;
    return NULL;

}

void sendBytes(int socket, char text[BUFFER_SIZE]) {
    int text_length = strlen(text);
    if (text_length >= BUFFER_SIZE) {
        fprintf(stderr, "Text too long to send\n");
        return;
    }

    strncpy(buffer, text, text_length);
    buffer[text_length] = '\0';

    if (send(socket, buffer, text_length, 0) == -1) {
        perror("Error sending data");
    }
}

void receiveBytes(int socket) {
    int bytes_received = recv(socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == -1) {
        perror("Error receiving data");
        return;
    }

    buffer[bytes_received] = '\0';
    printf("Server says: %s\n", buffer);
}




int startServer(int port) {

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        return 0;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        return 0;
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) == -1) {
        perror("Error listening for connections");
        return 0;
    }

    printf("Server listening on port %d\n", port);


    return 0;
}

void closeServer() {
    close(server_socket);
}

void closeClient(){
    close(client_socket);
}

int connectToServer(int port, char *ip, int *status) {
    struct sockaddr_in server_addr;
    //char buffer[BUFFER_SIZE];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        exit(1);
    }

    *status = 2;

    printf("Connected to server %s:%d\n", ip, port);

    return 0;
}



