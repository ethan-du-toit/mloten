#ifndef NODISPLAY
#include <SDL2/SDL.h>
#include "utility.h"
#include "display.h"

void display(SDL_Renderer* restrict renderer,
	     SDL_Surface* restrict surface, u8* restrict ram) {
  SDL_Colour palette[256];
  int offset = 0x461c00;
  for(int i = 0; i < 256; i++) {
    palette[i] = (SDL_Colour) {ram[offset + i*3], ram[offset + i*3 + 1], ram[offset + i*3 + 2], 0};
  }
  err_on_nonzero(SDL_SetPaletteColors(surface->format->palette, palette, 0, 256));
  err_on_nonzero(SDL_RenderClear(renderer));    
  SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surface);
  err_on_nonzero(SDL_RenderCopy(renderer,
				text,
				NULL,
				NULL));
  SDL_RenderPresent(renderer);
  SDL_DestroyTexture(text);  
}
#endif
