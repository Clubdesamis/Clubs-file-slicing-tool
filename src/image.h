#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct {
    SDL_Texture* texture;
    SDL_Rect rect;
    void* draw;
} Image;

void Image_init();