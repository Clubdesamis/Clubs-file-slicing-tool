#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define WIDTH 800
#define HEIGHT 600

int main(int argc, char** argv){

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* image;
    SDL_Rect rect;
    int width = 0, height = 0;


    window = SDL_CreateWindow("Clubwindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    image = IMG_LoadTexture(renderer, "/textures/fumo.png");
    SDL_QueryTexture(image, NULL, NULL, &width, &height);

    rect.x = WIDTH / 2;
    rect.y = HEIGHT / 2;
    rect.w = width;
    rect.h = height;

    while(1){

    }


    printf("Hello world");
    getchar();
    getchar();
    return 0;
}