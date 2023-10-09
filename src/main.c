#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "render.h"

// Constants
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 800
#define WINDOW_TITLE "Connect Four"
#define WINDOW_ICON_PATH "textures/icon.png"
#define FONT_PATH "fonts/IndieFlower-Regular.ttf"

#define COLOR_BACKGROUND 36, 36, 36, 255
#define COLOR_GRID 117, 117, 117, 117

#define ROW_MAX 6
#define COL_MAX 7
#define CELL_SIZE 80
#define GRID_OFFSET_X ((WINDOW_WIDTH - COL_MAX * CELL_SIZE) / 2)
#define GRID_OFFSET_Y ((WINDOW_HEIGHT - ROW_MAX * CELL_SIZE) / 2)

// SDL Resources
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *icon = NULL;
SDL_Texture *textures[3] = {NULL}; // Index 0 for player 1, 1 for player 2, 2 for preview
TTF_Font *font = NULL;
SDL_Color textColor = {255, 255, 255, 255};

int board[ROW_MAX][COL_MAX] = {0};
int current_player = 1;
int winner = 0;
int game_is_running = 0;

// Function to initialize SDL and other resources
int initialize() {
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

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Error creating SDL Window: %s\n", SDL_GetError());
        return 0;
    }

    icon = IMG_Load(WINDOW_ICON_PATH);
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

    return 1;
}

// Load textures for players and preview
int loadTextures() {
    const char *textureFiles[3] = {"textures/player_0.png", "textures/player_1.png", "textures/preview.png"};
    for (int i = 0; i < 3; i++) {
        textures[i] = IMG_LoadTexture(renderer, textureFiles[i]);
        if (!textures[i]) {
            fprintf(stderr, "Error loading texture %d: %s\n", i, IMG_GetError());
            return 0;
        }
    }
    return 1;
}

int loadFont() {
    font = TTF_OpenFont(FONT_PATH, 48);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont error: %s\n", TTF_GetError());
        return 0;
    }
    return 1;
}

// Initialize game resources
void setup() {

    if (!loadTextures()) {
        game_is_running = 0;
        return;
    }

    if (!loadFont()) {
        game_is_running = 0;
        return;
    }
}

// Clean up resources and quit the game
void quit() {
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int dropPiece(int col) {
    for (int row = ROW_MAX - 1; row >= 0; --row) {
        if (board[row][col] == 0 || board[row][col] == 3) {
            board[row][col] = current_player;
            return row;
        }
    }
    return -1; // Column is full
}

int checkWin(int player) {
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

// Process user input and events
void process() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            game_is_running = 0;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    game_is_running = 0;
                    break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {

                int mouseX = event.button.x;

                int gridX = (mouseX - GRID_OFFSET_X) / CELL_SIZE;

                if (gridX < 0 || gridX >= COL_MAX) {
                    break;
                }

                int gridY = dropPiece(gridX);
                if (gridY == -1) {
                    break;
                }

                board[gridY][gridX] = current_player;

                winner = checkWin(current_player);
                if (winner > 0) {
                    game_is_running = 0;
                }

                current_player = current_player == 1 ? 2 : 1;


                gridY = dropPiece(gridX);
                if (gridY == -1) {
                    break;
                }

                for (int row = 0; row < ROW_MAX; row++) {
                    for (int col = 0; col < COL_MAX; col++) {
                        if (board[row][col] == 3) {
                            board[row][col] = 0;
                        }
                    }
                }
                board[gridY][gridX] = 3;
            }
            break;
        case SDL_MOUSEMOTION:

            int mouseX = event.button.x;

            int gridX = (mouseX - GRID_OFFSET_X) / CELL_SIZE;

            if (gridX < 0 || gridX >= COL_MAX) {
                break;
            }

            int gridY = dropPiece(gridX);
            if (gridY == -1) {
                break;
            }

            for (int row = 0; row < ROW_MAX; row++) {
                for (int col = 0; col < COL_MAX; col++) {
                    if (board[row][col] == 3) {
                        board[row][col] = 0;
                    }
                }
            }
            board[gridY][gridX] = 3;
            break;
    }
}

// Update game logic
void update() {
}


// Render game graphics
void render() {

    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND);
    SDL_RenderClear(renderer);

    for (int row = 0; row < ROW_MAX; row++) {
        for (int col = 0; col < COL_MAX; col++) {

            SDL_Rect cellRect = {GRID_OFFSET_X + col * CELL_SIZE, GRID_OFFSET_Y + row * CELL_SIZE, CELL_SIZE,
                                 CELL_SIZE};

            SDL_SetRenderDrawColor(renderer, COLOR_GRID);
            SDL_RenderDrawRect(renderer, &cellRect);

            SDL_RenderCopy(renderer, textures[board[row][col] - 1], NULL, &cellRect);

        }
    }

    char *subtitle = current_player == 1 ? "It's your turn" : "Opponent's turn";
    drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 10, subtitle, &font, &textColor);

    SDL_Rect stateRect = {WINDOW_WIDTH / 20, WINDOW_HEIGHT - WINDOW_HEIGHT / 10 - CELL_SIZE / 2, CELL_SIZE, CELL_SIZE};
    SDL_RenderCopy(renderer, textures[current_player - 1], NULL, &stateRect);

    SDL_RenderPresent(renderer);
}

int main() {
    game_is_running = initialize();

    setup();

    while (game_is_running) {
        process();
        update();
        render();
    }

    if (winner != 0) {
        game_is_running = 1;
        while (game_is_running) {
            SDL_Event event;
            SDL_PollEvent(&event);
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                game_is_running = 0;
            } else if (event.type == SDL_QUIT) {
                game_is_running = 0;
            }

            SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND);
            SDL_RenderClear(renderer);

            char *subtitle = winner == 1 ? "Player 1 won! " : "Player 2 won!";
            drawText(renderer, WINDOW_WIDTH / 3, WINDOW_HEIGHT - WINDOW_HEIGHT / 2, subtitle, &font, &textColor);
            SDL_Rect titleRect = {WINDOW_WIDTH / 6, WINDOW_HEIGHT - WINDOW_HEIGHT / 2 - CELL_SIZE / 2, CELL_SIZE,
                                  CELL_SIZE};
            SDL_RenderCopy(renderer, textures[winner - 1], NULL, &titleRect);

            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 9, "Press 'esc' to quit!", &font,
                     &textColor);

            SDL_RenderPresent(renderer);

        }
    }


    quit();
    return 0;
}
