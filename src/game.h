#pragma once

#include "consts.h"

extern int status;

void init_game(const int player);

void gameTick();

const int isGameRunning();

void setGameRunning(const int status);

void handleMouseDown(const int mouse_x, const int mouse_y);

void process_sdl_input();

const int getPlayerId();

void dropPieceAtX(const int gridX, const int player);

const int getTopPos(const int board[ROW_MAX][COL_MAX], const int col);

void restartGame();