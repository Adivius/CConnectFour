#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "render.h"
#include "server.h"
#include "client.h"
#include "utils.h"
#include "game.h"


// Clean up resources and quit the game
void quit() {
    status = 0;
    closeClient();
    if (getPlayerId() == 1) {
        closeServer();
    }
    renderFree();
    exit(0);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        puts(USAGE);
        return 0;
    }

    int result;
    if ((result = init_sdl()) == 0) {
        exit(1);
    }
    init_game(result, argc);
    status = 1;

    if (argc == 2) {
        startServer(atoi(argv[1]));
        connectToServer(atoi(argv[1]), "127.0.0.1");
        setWindowTitle("Connect Four - Player 1 (Server)");
    } else {
        char resolvedIP[INET_ADDRSTRLEN];
        if (hostnameToIp(argv[2], resolvedIP, INET_ADDRSTRLEN) != 0){
            puts("Invalid ip\n");
            exit(1);
        }
        connectToServer(atoi(argv[1]), resolvedIP);
        status = 2;
        setWindowTitle("Connect Four - Player 2 (Client)");
    }

    while (isGameRunning()) {
        gameTick();
    }

    quit();
    return 0;
}