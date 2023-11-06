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

    if (argc < 2 || argc > 3) {
        printf("%s\n", USAGE);
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
        connectToServer(atoi(argv[1]), LOCALHOST);
        setWindowTitle(WINDOW_TITLE_SERVER);
    } else {
        char resolvedIP[INET_ADDRSTRLEN];
        if (hostnameToIp(argv[2], resolvedIP, INET_ADDRSTRLEN) != 0){
            printf("%s\n", "Invalid ip");
            quit();
        }
        connectToServer(atoi(argv[1]), resolvedIP);
        status = 2;
        setWindowTitle(WINDOW_TITLE_CLIENT);
    }

    while (isGameRunning()) {
        gameTick();
    }

    quit();
    return 0;
}