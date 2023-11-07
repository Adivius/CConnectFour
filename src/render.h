#pragma once

#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#include "consts.h"

const int init_sdl();

void setWindowTitle(const char *title);

void render_sdl(const int board[ROW_MAX][COL_MAX], const int winner, const int id_player, const int wins_you, const int wins_opp,
            const int current_player);

void drawText(SDL_Renderer *renderer, const int x, const int y, const char *text, TTF_Font **font,
              const SDL_Color *textColor);

void renderFree();