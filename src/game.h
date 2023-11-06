#include "utils.h"

int checkWin(int board[ROW_MAX][COL_MAX],  int player) {
    // Check horizontally
    for (int row = 0; row < ROW_MAX; ++row) {
        for (int col = 0; col < COL_MAX - 3; ++col) {
            if (board[row][col] == player &&
                board[row][col + 1] == player &&
                board[row][col + 2] == player &&
                board[row][col + 3] == player) {
                return player;
            }
        }
    }

    // Check vertically
    for (int row = 0; row < ROW_MAX - 3; ++row) {
        for (int col = 0; col < COL_MAX; ++col) {
            if (board[row][col] == player &&
                board[row + 1][col] == player &&
                board[row + 2][col] == player &&
                board[row + 3][col] == player) {
                return player;
            }
        }
    }

    // Check diagonally (top-left to bottom-right)
    for (int row = 0; row < ROW_MAX - 3; ++row) {
        for (int col = 0; col < COL_MAX - 3; ++col) {
            if (board[row][col] == player &&
                board[row + 1][col + 1] == player &&
                board[row + 2][col + 2] == player &&
                board[row + 3][col + 3] == player) {
                return player;
            }
        }
    }

    // Check diagonally (top-right to bottom-left)
    for (int row = 0; row < ROW_MAX - 3; ++row) {
        for (int col = 3; col < COL_MAX; ++col) {
            if (board[row][col] == player &&
                board[row + 1][col - 1] == player &&
                board[row + 2][col - 2] == player &&
                board[row + 3][col - 3] == player) {
                return player;
            }
        }
    }

    return 0; // No winner yet
}

int dropPiece(int board[ROW_MAX][COL_MAX], int col) {
    for (int row = ROW_MAX - 1; row >= 0; --row) {
        if (board[row][col] == 0 || board[row][col] == 3) {
            return row;
        }
    }
    return -1; // Column is full
}