#include "game.h"
#include "consts.h"
#include "render.h"
#include "client.h"

int board[ROW_MAX][COL_MAX] = {0};
int id_player, starter_player = 1, current_player = 1, winner, wins_you, wins_opp;
int game_is_running, status;

void init_game(const int argc) {
    game_is_running = 1;
    current_player = starter_player;
    id_player = argc == 2 ? 1 : 2;
}

const int isGameRunning() {
    return game_is_running;
}

void setGameRunning(int status) {
    game_is_running = status;
}

const int getPlayerId() {
    return id_player;
}

void gameTick() {
    process_sdl_input();
    render_sdl(board, winner, id_player, wins_you, wins_opp, current_player);
}

// Process user input and events
void process_sdl_input() {
    for (SDL_Event event; SDL_PollEvent(&event);) {
    switch (event.type) {
        case SDL_QUIT:
            setGameRunning(0);
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    setGameRunning(0);
                    break;
                case SDLK_r:
                    if (status != STATUS_STOPPING) {
                        return;
                    }
                    sendByteToServer(PACKET_REMATCH);
                    restartGame();
                    break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:

            if (event.button.button != SDL_BUTTON_LEFT || status != STATUS_RUNNING) {
                return;
            }
            handleMouseDown(event.button.x, event.button.y);
            break;
    }
}
}

const int checkWin() {
    // Check horizontally
    for (int row = 0; row < ROW_MAX; ++row) {
        for (int col = 0; col < COL_MAX - 3; ++col) {
            if (board[row][col] == current_player &&
                board[row][col + 1] == current_player &&
                board[row][col + 2] == current_player &&
                board[row][col + 3] == current_player) {
                return current_player;
            }
        }
    }

    // Check vertically
    for (int row = 0; row < ROW_MAX - 3; ++row) {
        for (int col = 0; col < COL_MAX; ++col) {
            if (board[row][col] == current_player &&
                board[row + 1][col] == current_player &&
                board[row + 2][col] == current_player &&
                board[row + 3][col] == current_player) {
                return current_player;
            }
        }
    }

    // Check diagonally (top-left to bottom-right)
    for (int row = 0; row < ROW_MAX - 3; ++row) {
        for (int col = 0; col < COL_MAX - 3; ++col) {
            if (board[row][col] == current_player &&
                board[row + 1][col + 1] == current_player &&
                board[row + 2][col + 2] == current_player &&
                board[row + 3][col + 3] == current_player) {
                return current_player;
            }
        }
    }

    // Check diagonally (top-right to bottom-left)
    for (int row = 0; row < ROW_MAX - 3; ++row) {
        for (int col = 3; col < COL_MAX; ++col) {
            if (board[row][col] == current_player &&
                board[row + 1][col - 1] == current_player &&
                board[row + 2][col - 2] == current_player &&
                board[row + 3][col - 3] == current_player) {
                return current_player;
            }
        }
    }

    return 0; // No winner yet
}

void handleMouseDown(const int mouse_x, const int mouse_y) {
    if (current_player != id_player) {
        return;
    }

    if (mouse_x < GRID_OFFSET_X || mouse_x > GRID_OFFSET_X + CELL_SIZE * COL_MAX ||
        mouse_y < GRID_OFFSET_Y || mouse_y > GRID_OFFSET_Y + CELL_SIZE * ROW_MAX) {
        return;
    }

    int gridX = (mouse_x - GRID_OFFSET_X) / CELL_SIZE;

    int gridY = getTopPos(board, gridX);
    if (gridY == -1) {
        return;
    }

    dropPieceAtX(gridX);

    sendByteToServer(gridX);
}

const int getTopPos(const int board[ROW_MAX][COL_MAX], int col) {
    for (int row = ROW_MAX - 1; row >= 0; --row) {
        if (board[row][col] == 0 || board[row][col] == 3) {
            return row;
        }
    }
    return -1; // Column is full
}

void restartGame() {
    starter_player = starter_player == 1 ? 2 : 1;
    current_player = starter_player;
    
    memset(board, 0, sizeof(board));

    status = STATUS_RUNNING;
}

void dropPieceAtX(const int gridX) {
    if (status != STATUS_RUNNING) {
        return;
    }

    int gridY = getTopPos(board, gridX);
    if (gridY == -1) {
        return;
    }

    board[gridY][gridX] = current_player;

    winner = checkWin();

    if (winner > 0) {
        printf("Winner %d\n", winner);
        if (winner == id_player) {
            wins_you++;
        } else {
            wins_opp++;
        }

        //For evil people: (*((winner == id_player) ? &wins_you : &wins_opp))++;

        status = STATUS_STOPPING;
    }

    current_player = current_player == 1 ? 2 : 1;
}