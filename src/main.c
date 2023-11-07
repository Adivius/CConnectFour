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

    if (!init_sdl()) {
        exit(1);
    }

    const int id_player = argc == 2 ? 1 : 2;

    init_game(id_player);
    status = STATUS_STARTING;

    setWindowTitle((id_player == 1) ? WINDOW_TITLE_SERVER : WINDOW_TITLE_CLIENT);

    const int port = atoi(argv[1]);

    if (id_player == 1) {
        startServer(port);
        connectToServer(port, 0);
    } else {
        char resolvedIP[INET_ADDRSTRLEN];
        if (resolveHostnameToIp(argv[2], resolvedIP) != 0){
            puts("Invalid ip");
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