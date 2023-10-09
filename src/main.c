#include <stdio.h>
#include <time.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

// Constants
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Connect Four"
#define WINDOW_ICON "textures/icon.png"

#define COLOR_BACKGROUND 19, 21, 22, 255
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

int board[ROW_MAX][COL_MAX] = {0};
int player_index = 1;
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

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Error creating SDL Window: %s\n", SDL_GetError());
        return 0;
    }

    icon = IMG_Load(WINDOW_ICON);
    if (!icon) {
        fprintf(stderr, "Error loading window icon surface: %s\n", IMG_GetError());
        return 0;
    }

    SDL_SetWindowIcon(window, icon);

    renderer = SDL_CreateRenderer(window, -1, 0);
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

// Initialize game resources
void setup() {
    srand(time(NULL));

    if (!loadTextures()) {
        game_is_running = 0;
    }
}

void printBoard() {
    for (int row = 0; row < ROW_MAX; row++) {
        for (int col = 0; col < COL_MAX; col++) {
            printf("%d ", board[row][col]);
        }
        printf("\n");
    }
}

int dropPiece(int col) {
    for (int row = ROW_MAX - 1; row >= 0; --row) {
        if (board[row][col] == 0 || board[row][col] == 3) {
            board[row][col] = player_index;
            return row;
        }
    }
    return -1; // Column is full
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

                board[gridY][gridX] = player_index;
                player_index = player_index == 1 ? 2 : 1;


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

            switch (board[row][col]) {
                case 1:
                    SDL_RenderCopy(renderer, textures[0], NULL, &cellRect);
                    break;
                case 2:
                    SDL_RenderCopy(renderer, textures[1], NULL, &cellRect);
                    break;
                case 3:
                    SDL_RenderCopy(renderer, textures[2], NULL, &cellRect);
                    break;
            }
        }
    }

    SDL_RenderPresent(renderer);
}

// Clean up resources and quit the game
void quit() {
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

int main() {
    game_is_running = initialize();
    setup();

    while (game_is_running) {
        process();
        update();
        render();
    }

    quit();
    return 0;
}
