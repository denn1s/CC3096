#include <iostream>
#include <SDL2/SDL.h>
#include "Game.h"
#include "TextureManager.h"

SDL_Texture* playerTex = NULL;
SDL_Rect srcRec, dstRec;

Game::Game()
{}

Game::~Game()
{}

void Game::init(const char* title, int width, int height)
{
  if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
  {
    std::cout << "SDL subsystems initialized" << std::endl;

    window = SDL_CreateWindow(title, 0, 0, width, height, 0);
    if (window)
    {
      std::cout << "SDL window created" << std::endl;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer)
    {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
      std::cout << "SDL renderer created" << std::endl;
    }

    isRunning = true;
  } else {
    isRunning = false;
  }
  counter = 0;

  playerTex = TextureManager::LoadTexture(renderer, "assets/player/front/frames/1.png");
}

void Game::handleEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0)
  {
    if (event.type == SDL_QUIT)
    {
      isRunning = false;
    }  
  }
}

void Game::update()
{
  counter++;
  
  dstRec.h = 80;
  dstRec.w = 80;
  dstRec.x = counter;
  
  //std::cout << counter << std::endl;
}

void Game::render()
{
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, playerTex, NULL, &dstRec);
  SDL_RenderPresent(renderer);
}

void Game::clean()
{
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  std::cout << "Game Over." << std::endl;
}

bool Game::running()
{
  return isRunning;
}
