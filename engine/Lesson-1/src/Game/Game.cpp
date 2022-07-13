#include <iostream>
#include <SDL2/SDL.h>
#include "Game.h"

Game::Game()
{
  FPS = 60;
  frameDuration = (1.0f / FPS) * 1000.0f;  // how many mili seconds in one frame
}

Game::~Game()
{}

void Game::init(const char* title, int width, int height)
{
  if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
  {
    window = SDL_CreateWindow(title, 0, 0, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 200, 255, 255, 1);
    std::cout << "Game Start!" << std::endl;

    isRunning = true;
  } else {
    isRunning = false;
  }
  counter = 0;
}


void Game::setup()
{
  std::cout << "Game Setup!" << std::endl;
}

void Game::frameStart()
{
  frameStartTimestamp = SDL_GetTicks();
}

void Game::frameEnd()
{
  frameEndTimestamp = SDL_GetTicks();

  float actualFrameDuration = frameEndTimestamp - frameStartTimestamp;

  if (actualFrameDuration < frameDuration)
  {
    SDL_Delay(frameDuration - actualFrameDuration);
  }

  counter++;
}

void Game::handleEvents()
{
  std::cout << "Game Handling events..." << std::endl;

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
  std::cout << "Game Update!" << std::endl;

}

void Game::render()
{
  std::cout << "Game Rendering" << std::endl;
  std::cout << "Frame:" << counter << std::endl;

  SDL_RenderClear(renderer);

  // actually render stuff

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
