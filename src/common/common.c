#include "common.h"
#include "constants.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Window* window;
SDL_Renderer* renderer;
char windowTitle[32];

void common_startup() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG);

    TTF_Init();
    window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, C_WINDOW_WIDTH, C_WINDOW_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
}

void common_shutdown() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
