#define SDL_MAIN_HANDLED
#include "common/common.h"
#include <dirent.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_FILE_SIZE 10000000


int main(int argc, char** argv){


    FILE* file = NULL;
    uint64_t fileSize = 0;
    char* content = NULL;
    char** subFiles = NULL;
    int subFileCount = 0;

    //ouvrir le fichier
    file = fopen("inputComplete/file.txt", "r");
    fseek(file, 0, SEEK_END);
    //trouver la taille du fichier
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    //allocate un tableau pour le contenu du fichier
    content = malloc(fileSize * sizeof(char));
    fgets(content, fileSize, file);

    subFileCount = (fileSize + MAX_FILE_SIZE - 1) / MAX_FILE_SIZE;
    subFiles = malloc(subFileCount * sizeof(char*));

    for(int i = 0; i < subFileCount; i++){
        subFiles[i] = (char*)malloc(MAX_FILE_SIZE * sizeof(char));
    }

    


    








    bool runs = true;

    SDL_Texture* image;
    char windowTitle[32];
    common_startup();
    TTF_Font* font = TTF_OpenFont("ressources/fonts/OpenSans-Light.ttf", 24);
    SDL_Surface* FpsLabelSurface = TTF_RenderText_Solid(font, "FPS: ", (SDL_Color){.r = 0, .g = 255, .b = 0, .a = 255});
    SDL_Texture* FpsLabelTexture = SDL_CreateTextureFromSurface(renderer, FpsLabelSurface);
    SDL_Rect FpsLabelRect;
    SDL_Rect fumoRect;
    int width = 0, height = 0;
    struct timespec res;
    long tickS, tickSOld, tickSSinceUpdate, tickNS, tickNSOld, tickNSSinceUpdate;
    clock_gettime(CLOCK_REALTIME, &res);
    tickNSOld = res.tv_nsec;
    tickNS = res.tv_nsec;
    tickNSSinceUpdate = res.tv_nsec;

    image = IMG_LoadTexture(renderer, "./fumo.png");
    SDL_QueryTexture(image, NULL, NULL, &width, &height);
    fumoRect.x = WIDTH / 2;
    fumoRect.y = HEIGHT / 2;
    fumoRect.w = width/2;
    fumoRect.h = height/2;
    FpsLabelRect.x = 20;
    FpsLabelRect.y = 20;
    FpsLabelRect.w = FpsLabelSurface->w;
    FpsLabelRect.h = FpsLabelSurface->h;
    
    sprintf(windowTitle, "FPS: %i", 0);
    SDL_SetWindowTitle(window, windowTitle);

    while(runs){

        printf("Ouais allo bassem");
        
        clock_gettime(CLOCK_REALTIME, &res);
        tickNS = res.tv_nsec;
        tickS = res.tv_sec;
        if((tickS * 1000000000 + tickNS) - (tickSSinceUpdate * 1000000000 + tickNSSinceUpdate) >= 500000000){
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
 


        SDL_RenderCopy(renderer, FpsLabelTexture, NULL, &FpsLabelRect);

        SDL_RenderPresent(renderer);
    }

    common_shutdown();


    return 0;
}