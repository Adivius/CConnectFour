#pragma once

#include "consts.h"

extern int status;

const int getTopPos(const int board[ROW_MAX][COL_MAX], const int col);

void restartGame();

void dropPieceAtX(const int gridX);

void init_game(const int argc);

const int isGameRunning();

void setGameRunning(const int status);

void gameTick();

const int getPlayerId();

void handleMouseDown(const int mouse_x, const int mouse_y);