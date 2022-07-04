#include "Game.h"

class TextureManager {
  public:
    static SDL_Texture* LoadTexture(SDL_Renderer* renderer, const char* fileName);
};