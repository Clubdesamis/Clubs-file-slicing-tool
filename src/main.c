#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
//#include <SDL2/SDL_ttf.h>

#define WIDTH 800
#define HEIGHT 600

int main(int argc, char** argv){

    bool runs = true;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* image;
    char windowTitle[32];

    //TTF_Font* font = TTF_OpenFont("Sans.ttf", 24);
    //SDL_Surface* FpsLabelSurface = TTF_RenderText_Solid(font, "FPS: ", (SDL_Color){0, 255, 0});
    //SDL_Surface* FpsIndicatorSurface = TTF_RenderText_Solid(font, "", (SDL_Color){0, 255, 0});
    //SDL_Texture* FpsLabelTexture = SDL_CreateTextureFromSurface(renderer, FpsLabelSurface);
    //SDL_Texture* FpsIndicatorTexture = SDL_CreateTextureFromSurface(renderer, FpsIndicatorSurface);
    //SDL_Rect FpsLabelRect;
    //SDL_Rect FpsIndicatorRect;
    SDL_Rect fumoRect;
    int width = 0, height = 0;
    struct timespec res;
    long tickS, tickSOld, tickSSinceUpdate, tickNS, tickNSOld, tickNSSinceUpdate;

    clock_gettime(CLOCK_REALTIME, &res);
    tickNSOld = res.tv_nsec;
    tickNS = res.tv_nsec;
    tickNSSinceUpdate = res.tv_nsec;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        return 1;
    }

    IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG);

    window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    image = IMG_LoadTexture(renderer, "./fumo.png");
    SDL_QueryTexture(image, NULL, NULL, &width, &height);

    fumoRect.x = WIDTH / 2;
    fumoRect.y = HEIGHT / 2;
    fumoRect.w = width;
    fumoRect.h = height;

    //FpsLabelRect.x = 20;
    //FpsLabelRect.y = 20;
    //SDL_QueryTexture(FpsLabelTexture, NULL, NULL, &FpsLabelRect.w, &FpsLabelRect.h);

    //FpsIndicatorRect.x = 160;
    //FpsIndicatorRect.y = 20;
    //SDL_QueryTexture(FpsLabelTexture, NULL, NULL, &FpsIndicatorRect.w, &FpsIndicatorRect.h);
    
    sprintf(windowTitle, "FPS: %i", 0);
    SDL_SetWindowTitle(window, windowTitle);



    while(runs){
        
        clock_gettime(CLOCK_REALTIME, &res);
        tickNS = res.tv_nsec;
        tickS = res.tv_sec;
        //printf("%ld\n", ticks);
        if((tickS * 1000000000 + tickNS) - (tickSSinceUpdate * 1000000000 + tickNSSinceUpdate) >= 500000000){
            //printf("fps updates");
            sprintf(windowTitle, "FPS: %i", 1000000000 / (tickNS - tickNSOld));
            SDL_SetWindowTitle(window, windowTitle);
            tickSSinceUpdate = tickS;
            tickNSSinceUpdate = tickNS;
        }
        tickSOld = tickS;
        tickNSOld = tickNS;

        SDL_Event e;
        while(SDL_PollEvent(&e)){
            switch(e.type){
                case(SDL_QUIT): {
                    runs = false;
                    break;
                }
                case(SDL_KEYDOWN): {
                    switch(e.key.keysym.sym){
                        case(SDLK_w): {
                            fumoRect.y-= 5;;
                            break;
                        }
                        case(SDLK_a): {
                            fumoRect.x-= 5;
                            break;
                        }
                        case(SDLK_s): {
                            fumoRect.y+= 5;
                            break;
                        }
                        case(SDLK_d): {
                            fumoRect.x+= 5;
                            break;
                        }
                    }
                    break;
                }

            }
        }

        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, image, NULL, &fumoRect);
        //SDL_RenderCopy(renderer, FpsLabelTexture, NULL, &FpsLabelRect);
        //SDL_RenderCopy(renderer, FpsIndicatorTexture, NULL, &FpsIndicatorRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}