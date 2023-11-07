#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#include "render.h"
#include "game.h"
#include "consts.h"
#include "server.h"
#include "client.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *icon;
SDL_Texture *textures[3]; // Index 0 for player 1, 1 for player 2, 2 for preview
TTF_Font *font;
SDL_Color textColor = {255, 255, 255, 255};

const int init_sdl() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }

    if (IMG_Init(IMG_INIT_PNG) < 0) {
        fprintf(stderr, "Error initializing IMG: %s\n", IMG_GetError());
        return 0;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Error creating SDL Window: %s\n", SDL_GetError());
        return 0;
    }

    icon = IMG_Load(ICON_PATH_WINDOW);
    if (!icon) {
        fprintf(stderr, "Error loading window icon surface: %s\n", IMG_GetError());
        return 0;
    }

    SDL_SetWindowIcon(window, icon);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer: %s\n", SDL_GetError());
        return 0;
    }

    //Load textures
    const char *textureFiles[3] = {ICON_PATH_PLAYER_ONE, ICON_PATH_PLAYER_TWO, ICON_PATH_PREVIEW};
    for (int i = 0; i < 3; i++) {
        textures[i] = IMG_LoadTexture(renderer, textureFiles[i]);
        if (!textures[i]) {
            fprintf(stderr, "Error loading texture %d: %s\n", i, IMG_GetError());
            return 0;
        }
    }

    //Load font
    font = TTF_OpenFont(FONT_PATH, 48);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont error: %s\n", TTF_GetError());
        return 0;
    }

    return 1;
}

void setWindowTitle(const char *title) {
    SDL_SetWindowTitle(window, title);
}

void drawText(SDL_Renderer *renderer, const int x,
              const int y,
              const char *text,
              TTF_Font **font,
              const SDL_Color *textColor) {

    //Draw with the according color
    SDL_Surface *surface =
            TTF_RenderText_Blended(*font, text, *textColor);

    //Draw to texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    //Adjust position
    SDL_Rect textRect;

    SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);

    textRect.x = x, textRect.y = y - textRect.h / 2;

    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_DestroyTexture(texture);
}

void render_sdl(const int board[ROW_MAX][COL_MAX], const int winner, const int id_player, const int wins_you, const int wins_opp,
            const int current_player) {

    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND);
    SDL_RenderClear(renderer);

    switch (status) {
        case STATUS_STOPPING:;
            char winner_text[40];
            sprintf(winner_text, "%s  %d : %d", winner == id_player ? "You won! " : "You lost!", wins_you, wins_opp);
            drawText(renderer, WINDOW_WIDTH / 3, WINDOW_HEIGHT / 8,
                     winner_text, &font, &textColor);
            SDL_Rect titleRect = {WINDOW_WIDTH / 6, WINDOW_HEIGHT / 8 - CELL_SIZE / 2, CELL_SIZE,
                                  CELL_SIZE};
            SDL_RenderCopy(renderer, textures[winner - 1], NULL, &titleRect);

            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 8, "Press 'esc' to quit", &font,
                     &textColor);
            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 14, "And 'R' to rematch!", &font,
                     &textColor);
        case STATUS_RUNNING:
            for (int row = 0; row < ROW_MAX; row++) {
                for (int col = 0; col < COL_MAX; col++) {

                    SDL_Rect cellRect = {GRID_OFFSET_X + col * CELL_SIZE, GRID_OFFSET_Y + row * CELL_SIZE, CELL_SIZE,
                                         CELL_SIZE};

                    SDL_SetRenderDrawColor(renderer, COLOR_GRID);
                    SDL_RenderDrawRect(renderer, &cellRect);

                    SDL_RenderCopy(renderer, textures[board[row][col] - 1], NULL, &cellRect);

                }
            }
            if (status == STATUS_STOPPING) {
                break;
            }

            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 10,
                     current_player == id_player ? "It's your turn" : "Opponent's turn", &font, &textColor);

            SDL_Rect stateRect = {WINDOW_WIDTH / 20, WINDOW_HEIGHT - WINDOW_HEIGHT / 10 - CELL_SIZE / 2, CELL_SIZE,
                                  CELL_SIZE};
            SDL_RenderCopy(renderer, textures[current_player - 1], NULL, &stateRect);
            break;
        case STATUS_STARTING:
            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 2, "Waiting for opponent", &font,
                     &textColor);
            break;
        default:
            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 2, "Error", &font, &textColor);
            break;


    }
    SDL_RenderPresent(renderer);

}


void renderFree() {
    for (int i = 0; i < sizeof(textures) / sizeof(textures[0]); i++) {
        SDL_DestroyTexture(textures[i]);
    }
    SDL_FreeSurface(icon);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
