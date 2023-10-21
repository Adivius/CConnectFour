#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "render.h"
#include "server.h"
#include "client.h"
#include "updater.h"

#define USAGE "\033[0;31mUsgae: ./cconnectfour <port> <ip>"

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
int id_player = 0;
int current_player = 1;
int winner = 0;
int game_is_running = 0;
int status = 0; // 0=Error(default), 1=Starting, 2=Playing, 3=Ending

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

    //Load textures
    const char *textureFiles[3] = {"textures/player_1.png", "textures/player_2.png", "textures/preview.png"};
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

    status = 1;
    current_player = 1;
    return 1;
}

// Clean up resources and quit the game
void quit() {
    closeClient();
    if (id_player == 1) {
        closeClients();
        closeServer();
    }
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(textures[i]);
    }
    SDL_FreeSurface(icon);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    exit(0);
}

int dropPiece(int col) {
    for (int row = ROW_MAX - 1; row >= 0; --row) {
        if (board[row][col] == 0 || board[row][col] == 3) {
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

    if (SDL_PollEvent(&event) == 0) {
        return;
    }

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

            if (event.button.button != SDL_BUTTON_LEFT) {
                return;
            }

            if (status != 2) {
                return;
            }

            if (current_player != id_player) {
                return;
            }

            int mouseX = event.button.x;

            int gridX = (mouseX - GRID_OFFSET_X) / CELL_SIZE;

            if (gridX < 0 || gridX >= COL_MAX) {
                return;
            }

            int gridY = dropPiece(gridX);
            if (gridY == -1) {
                return;
            }

            update(gridX);

            sendByteToServer(gridX + '0');


            printf("GEDRÜCKT, Jetzt ist %d dran\n", current_player);
            break;
    }
}

// Update game logic
void update(int gridX) {
    if (status != 2) {
        return;
    }

    int gridY = dropPiece(gridX);
    if (gridY == -1) {
        return;
    }

    board[gridY][gridX] = current_player;

    winner = checkWin(current_player);
    if (winner > 0) {
        printf("Winner %d\n", winner);
        status = 3;
    }


    current_player = current_player == 1 ? 2 : 1;
}


// Render game graphics
void render() {

    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND);
    SDL_RenderClear(renderer);

    switch (status) {
        case 1:
            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 2, "Waiting for opponent", &font,
                     &textColor);
            break;
        case 2:
            for (int row = 0; row < ROW_MAX; row++) {
                for (int col = 0; col < COL_MAX; col++) {

                    SDL_Rect cellRect = {GRID_OFFSET_X + col * CELL_SIZE, GRID_OFFSET_Y + row * CELL_SIZE, CELL_SIZE,
                                         CELL_SIZE};

                    SDL_SetRenderDrawColor(renderer, COLOR_GRID);
                    SDL_RenderDrawRect(renderer, &cellRect);

                    SDL_RenderCopy(renderer, textures[board[row][col] - 1], NULL, &cellRect);

                }
            }

            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 10,
                     current_player == id_player ? "It's your turn" : "Opponent's turn", &font, &textColor);

            SDL_Rect stateRect = {WINDOW_WIDTH / 20, WINDOW_HEIGHT - WINDOW_HEIGHT / 10 - CELL_SIZE / 2, CELL_SIZE,
                                  CELL_SIZE};
            SDL_RenderCopy(renderer, textures[current_player - 1], NULL, &stateRect);
            break;
        case 3:
            drawText(renderer, WINDOW_WIDTH / 3, WINDOW_HEIGHT - WINDOW_HEIGHT / 2,
                     winner == 1 ? "Player 1 won! " : "Player 2 won!", &font, &textColor);
            SDL_Rect titleRect = {WINDOW_WIDTH / 6, WINDOW_HEIGHT - WINDOW_HEIGHT / 2 - CELL_SIZE / 2, CELL_SIZE,
                                  CELL_SIZE};
            SDL_RenderCopy(renderer, textures[winner - 1], NULL, &titleRect);

            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 9, "Press 'esc' to quit!", &font,
                     &textColor);
            break;

        default:
            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 2, "Error", &font, &textColor);
            break;


    }
    SDL_RenderPresent(renderer);

}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        puts(USAGE);
        return 0;
    }

    game_is_running = initialize();

    if (argc == 2) {
        id_player = 1;
        startServer(atoi(argv[1]));
        connectToServer(atoi(argv[1]), "127.0.0.1");
    } else {
        id_player = 2;
        connectToServer(atoi(argv[1]), argv[2]);
        status = 2;
    }

    while (game_is_running) {
        process();
        render();
    }

    quit();
    return 0;
}