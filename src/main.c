#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "render.h"
#include "server.h"
#include "client.h"
#include "utils.h"
#include "game.h"


// SDL Resources
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *icon = NULL;
SDL_Texture *textures[3] = {NULL}; // Index 0 for player 1, 1 for player 2, 2 for preview
TTF_Font *font = NULL;
SDL_Color textColor = {255, 255, 255, 255};

int board[ROW_MAX][COL_MAX] = {0};
int id_player = 0, starter_player = 1, current_player = 1, winner = 0, wins_you = 0, wins_opp = 0;
int game_is_running = 0, status = 0; // 0=Error(default), 1=Starting, 2=Playing, 3=Ending

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

    status = 1;
    current_player = starter_player;
    return 1;
}

void setWindowTitle(char* title){
    SDL_SetWindowTitle(window, title);
}

// Clean up resources and quit the game
void quit() {
    status = 0;
    closeClient();
    if (id_player == 1) {
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
                case SDLK_r:
                    if (status != 3){
                        return;
                    }
                    char temp = (7 + id_player) + '0';
                    sendByteToServer(temp);
                    reset();
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
            int mouseY = event.button.y;

            if (mouseX < GRID_OFFSET_X || mouseX > GRID_OFFSET_X + CELL_SIZE * COL_MAX || mouseY < GRID_OFFSET_Y || mouseY > GRID_OFFSET_Y + CELL_SIZE * ROW_MAX) {
                return;
            }

            int gridX = (mouseX - GRID_OFFSET_X) / CELL_SIZE;

            int gridY = dropPiece(board, gridX);
            if (gridY == -1) {
                return;
            }

            update(gridX);

            sendByteToServer(gridX + '0');

            break;
    }
}

// Update game logic
void update(int gridX) {
    if (status != 2) {
        return;
    }

    int gridY = dropPiece(board, gridX);
    if (gridY == -1) {
        return;
    }

    board[gridY][gridX] = current_player;

    winner = checkWin(board, current_player);

    if (winner > 0) {
        printf("Winner %d\n", winner);
        if (winner == id_player){
            wins_you++;
        } else{
            wins_opp++;
        }
        status = 3;
    }


    current_player = current_player == 1 ? 2 : 1;
}

void reset(){
    starter_player = starter_player == 1 ? 2 : 1;
    current_player = starter_player;
    memset(board, 0, sizeof(board[0][0]) * ROW_MAX * COL_MAX);
    status = 2;
}


// Render game graphics
void render() {

    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND);
    SDL_RenderClear(renderer);

    switch (status) {
        case 3:
            char winner_text[40];
            sprintf(winner_text, "%s  %d : %d", winner == id_player ? "You won! " : "You lost!", wins_you, wins_opp);
            drawText(renderer, WINDOW_WIDTH / 3,  WINDOW_HEIGHT / 8,
                     winner_text, &font, &textColor);
            SDL_Rect titleRect = {WINDOW_WIDTH / 6, WINDOW_HEIGHT / 8 - CELL_SIZE / 2, CELL_SIZE,
                                  CELL_SIZE};
            SDL_RenderCopy(renderer, textures[winner - 1], NULL, &titleRect);

            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 8, "Press 'esc' to quit", &font,
                     &textColor);
            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 14, "And 'R' to rematch!", &font,
                     &textColor);
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
            if (status == 3){
                break;
            }

            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 10,
                     current_player == id_player ? "It's your turn" : "Opponent's turn", &font, &textColor);

            SDL_Rect stateRect = {WINDOW_WIDTH / 20, WINDOW_HEIGHT - WINDOW_HEIGHT / 10 - CELL_SIZE / 2, CELL_SIZE,
                                  CELL_SIZE};
            SDL_RenderCopy(renderer, textures[current_player - 1], NULL, &stateRect);
            break;
        case 1:
            drawText(renderer, WINDOW_WIDTH / 5, WINDOW_HEIGHT - WINDOW_HEIGHT / 2, "Waiting for opponent", &font,
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

    id_player = argc == 2 ? 1 : 2;

    if (argc == 2) {
        startServer(atoi(argv[1]));
        connectToServer(atoi(argv[1]), "127.0.0.1");
        setWindowTitle("Connect Four - Player 1 (Server)");
    } else {
        char resolvedIP[INET_ADDRSTRLEN];
        if (hostnameToIp(argv[2], resolvedIP, INET_ADDRSTRLEN) != 0){
            puts("Invalid ip\n");
            exit(1);
        }
        connectToServer(atoi(argv[1]), resolvedIP);
        status = 2;
        setWindowTitle("Connect Four - Player 2 (Client)");
    }

    while (game_is_running) {
        process();
        render();
    }

    quit();
    return 0;
}