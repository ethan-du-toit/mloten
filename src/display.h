#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL2/SDL.h>
#include "utility.h"

inline void display(SDL_Renderer* restrict renderer, SDL_Surface* restrict surface, u8* restrict ram);
#endif
