#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#include "utils.h"

void renderFree();

void render(int status, int board[ROW_MAX][COL_MAX], int winner, int id_player, int wins_you, int wins_opp,
            int current_player);

void drawText(SDL_Renderer *renderer, const int x, const int y, const char *text, TTF_Font **font,
              const SDL_Color *textColor);

void setWindowTitle(char *title);

int init_sdl();