#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

void drawText(SDL_Renderer *renderer,
              const int x,
              const int y,
              const char *text,
              TTF_Font **font,
              const SDL_Color *textColor) {

    //Draw with the according color
    SDL_Surface *surface =
            TTF_RenderText_Solid(*font, text, *textColor);

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