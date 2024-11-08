#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

int main(int argc, char** argv){

    SDL_Window* window;
    window = SDL_CreateWindow("Clubwindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);

    printf("Hello world");
    getchar();
    getchar();
    return 0;
}