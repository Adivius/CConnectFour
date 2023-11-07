#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "render.h"
#include "server.h"
#include "client.h"
#include "consts.h"
#include "game.h"

// Clean up resources and quit the game
void quit() {
    status = STATUS_ERROR;
    closeClient();
    if (getPlayerId() == 1) {
        closeServer();
    }
    renderFree();
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        puts(USAGE);
        return 0;
    }

    int result;
    if ((result = init_sdl()) == 0) {
        exit(1);
    }
    init_game(argc);
    status = STATUS_STARTING;

    setWindowTitle((argc == 2) ? WINDOW_TITLE_SERVER : WINDOW_TITLE_CLIENT);

    const int port = atoi(argv[1]);

    if (argc == 2) {
        startServer(port);
        connectToServer(port, 0);
    } else {
        char resolvedIP[INET_ADDRSTRLEN];
        if (hostnameToIp(argv[2], resolvedIP, INET_ADDRSTRLEN) != 0){
            printf("%s\n", "Invalid ip");
            quit();
        }
        connectToServer(port, inet_addr(resolvedIP));
        status = STATUS_RUNNING;
    }

    while (isGameRunning()) {
        gameTick();
    }

    quit();
}