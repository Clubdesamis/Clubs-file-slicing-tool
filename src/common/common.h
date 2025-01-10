#pragma once

#include "includes.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern char windowTitle[32];

void common_startup();

void common_shutdown();