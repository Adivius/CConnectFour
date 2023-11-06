#include "consts.h"

int dropPiece(int board[ROW_MAX][COL_MAX], int col);

void reset();

void update(int gridX);

void init_game(int result, int argc);

int isGameRunning();

void setGameRunning(int status);

void gameTick();

int getPlayerId();

void handleMouseDown(int mouse_x, int mouse_y);