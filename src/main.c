#include <stdio.h>
#include <time.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#define COLOR_BACKGROUND 19, 21, 22, 255  // Background color in RGBA format

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

#define randNum(min, max) ((rand() % (int)(((max) + 1) - (min))) + (min))
#define randBool randNum(0, 1)

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *surface1 = NULL;
SDL_Texture *texture1 = NULL;

int game_is_running = 0;  // Flag to control the game loop
int last_frame_time = 0;  // Store the time of the last frame

// Function to initialize SDL and create the window and renderer
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

// Function to load an image and create a texture from it
void setup() {
    srand(time(NULL));

    surface1 = IMG_Load("textures/player_1.png");
    if (!surface1) {
        fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
        game_is_running = 0;
        return;
    }

    texture1 = SDL_CreateTextureFromSurface(renderer, surface1);
    if (!texture1) {
        fprintf(stderr, "Error creating SDL texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface1);
        game_is_running = 0;
        return;
    }

    SDL_FreeSurface(surface1);
}

// Function to process SDL events (e.g., quitting the game)
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
    }
}

// Function to update the game state (not used in this example)
void update() {
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();
}

// Function to render the game scene
void render() {
    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND);
    SDL_RenderClear(renderer);

    int textureWidth, textureHeight;
    SDL_QueryTexture(texture1, NULL, NULL, &textureWidth, &textureHeight);

    SDL_Rect destRect = {0, 0, textureWidth, textureHeight};
    SDL_RenderCopy(renderer, texture1, NULL, &destRect);

    SDL_RenderPresent(renderer);
}

// Function to clean up resources and quit SDL
void quit() {
    SDL_DestroyTexture(texture1);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Main function
int main() {
    game_is_running = initialize();  // Initialize SDL

    setup();  // Load image and create texture

    while (game_is_running) {
        process();  // Process SDL events
        update();   // Update game state (not used in this example)
        render();   // Render the game scene
    }

    quit();  // Clean up resources and quit SDL

    return 0;
}
