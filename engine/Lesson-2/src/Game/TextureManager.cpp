#include "TextureManager.h"

SDL_Texture* TextureManager::LoadTexture(SDL_Renderer* renderer, const char* fileName)
{
  SDL_Surface* tmpSur = IMG_Load(fileName);
  SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tmpSur);
  SDL_FreeSurface(tmpSur);
  return tex;
}