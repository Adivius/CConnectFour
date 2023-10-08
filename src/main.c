#include <stdio.h>
#include <time.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

// Define constants
#define COLOR_BACKGROUND 19, 21, 22, 255
#define MAX_PIECES 42
#define GRID_SIZE 80 // Size of the grid in pixels

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

// Declare global variables and SDL resources
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;

// Define a structure to hold game pieces
typedef struct {
    SDL_Texture *texture;
    SDL_Rect position;
} GAME_Piece;

GAME_Piece pieces[MAX_PIECES];

// Initialize control variables
int instruction = 0;
int player_index = 0;
int piece_count = 0;
int game_is_running = 0;
int last_frame_time = 0;

// Function to initialize SDL and other resources
int initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL\n");
        printf("%s", SDL_GetError());
        return 0;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        fprintf(stderr, "Error initializing IMG\n");
        return 0;
    }

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Error creating SDL Window: %s\n", SDL_GetError());
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

// Function to load the game texture based on the player index
void loadTexture(int index) {
    switch (index) {
        case 1:
            texture = IMG_LoadTexture(renderer, "textures/player_1.png");
            break;
        default:
            texture = IMG_LoadTexture(renderer, "textures/player_0.png");;
            break;
    }
    if (!texture) {
        fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
        game_is_running = 0;
        return;
    }
}

// Initialize game resources
void setup() {
    srand(time(NULL));

    loadTexture(1);
}

// Function to snap a position to the grid
void snapToGrid(SDL_Rect *position) {
    // Calculate the nearest grid position by adding GRID_SIZE / 2 to center it
    // and then dividing by GRID_SIZE, ensuring it snaps to the grid
    position->x = ((position->x + GRID_SIZE / 2) / GRID_SIZE) * GRID_SIZE;
    position->y = ((position->y + GRID_SIZE / 2) / GRID_SIZE) * GRID_SIZE;
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
                case SDLK_r:
                    piece_count = 0;
                    instruction = 1;
                    break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:

            if (event.button.button != SDL_BUTTON_LEFT) {
                break;
            }

            if (piece_count < MAX_PIECES) {
                player_index = !player_index;

                loadTexture(player_index);

                GAME_Piece piece;
                piece.texture = texture;
                int textureWidth, textureHeight;
                SDL_QueryTexture(piece.texture, NULL, NULL, &textureWidth, &textureHeight);
                SDL_GetMouseState(&piece.position.x, &piece.position.y);

                // Snap the piece's position to the grid
                snapToGrid(&piece.position);

                piece.position.w = textureWidth;
                piece.position.h = textureHeight;
                pieces[piece_count] = piece;
                piece_count++;
            }

            break;
    }
}

// Update game logic
void update() {
//    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
//    last_frame_time = SDL_GetTicks();
}

// Render game graphics
void render() {
    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND);
    SDL_RenderClear(renderer);

    if (instruction == 0) {
        for (int i = 0; i < piece_count; i++) {
            SDL_RenderCopy(renderer, pieces[i].texture, NULL, &pieces[i].position);
        }
    } else {
        instruction = 0;
    }

    SDL_RenderPresent(renderer);
}

// Clean up resources and quit the game
void quit() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
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
