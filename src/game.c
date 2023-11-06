#include "game.h"
#include "utils.h"
#include "render.h"
#include "client.h"

int board[ROW_MAX][COL_MAX] = {0};
int id_player = 0, starter_player = 1, current_player = 1, winner = 0, wins_you = 0, wins_opp = 0;
int game_is_running = 0, status = 0; // 0=Error(default), 1=Starting, 2=Playing, 3=Ending

void init_game(int result, int argc) {
    game_is_running = result;
    current_player = starter_player;
    id_player = argc == 2 ? 1 : 2;
}

int isGameRunning() {
    return game_is_running;
}

void setGameRunning(int status) {
    game_is_running = status;
}

int getPlayerId() {
    return id_player;
}

void gameTick() {
    process();
    render(status, board, winner, id_player, wins_you, wins_opp, current_player);
}

int checkWin() {
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

void handleMouseDown(int mouse_x, int mouse_y) {

    if (current_player != id_player) {
        return;
    }

    if (mouse_x < GRID_OFFSET_X || mouse_x > GRID_OFFSET_X + CELL_SIZE * COL_MAX ||
        mouse_y < GRID_OFFSET_Y || mouse_y > GRID_OFFSET_Y + CELL_SIZE * ROW_MAX) {
        return;
    }

    int gridX = (mouse_x - GRID_OFFSET_X) / CELL_SIZE;

    int gridY = dropPiece(board, gridX);
    if (gridY == -1) {
        return;
    }

    update(gridX);

    sendByteToServer(gridX + '0');
}

int dropPiece(int board[ROW_MAX][COL_MAX], int col) {
    for (int row = ROW_MAX - 1; row >= 0; --row) {
        if (board[row][col] == 0 || board[row][col] == 3) {
            return row;
        }
    }
    return -1; // Column is full
}

void reset() {
    starter_player = starter_player == 1 ? 2 : 1;
    current_player = starter_player;
    memset(board, 0, sizeof(board[0][0]) * ROW_MAX * COL_MAX);
    status = 2;
}

void update(int gridX) {
    if (status != 2) {
        return;
    }

    int gridY = dropPiece(board, gridX);
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
        status = 3;
    }


    current_player = current_player == 1 ? 2 : 1;
}