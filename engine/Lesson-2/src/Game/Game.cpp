#include <iostream>
#include <SDL2/SDL.h>
#include "Game.h"

int screen_width;
int screen_height;

SDL_Rect ball;
SDL_Rect paddle;

Game::Game()
{
  FPS = 240;
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
    std::cout << "Game Start!" << std::endl;

    screen_width = width;
    screen_height = height;

    isRunning = true;
  } else {
    isRunning = false;
  }
  counter = 0;
}


void Game::setup()
{
  ball.x = 20;
  ball.y = 20;
  ball.w = 15;
  ball.h = 15;

  paddle.x = (screen_width / 2) - 50;
  paddle.y = screen_height - 20;
  paddle.w = 100;
  paddle.h = 20;
}

void Game::frameStart()
{
  std::cout << "---- Frame: " << counter << " ----" << std::endl;
  frameStartTimestamp = SDL_GetTicks();
  dT = frameEndTimestamp - frameStartTimestamp;
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
  std::cout << " " << std::endl;
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

    if (event.type == SDL_KEYDOWN)
    {
      switch (event.key.keysym.sym)
      {
        case SDLK_LEFT:
          paddle.x -= 10;
          break;
        case SDLK_RIGHT:
          paddle.x += 10;
          break;
      }
    }
  }
}

int sx = 1;
int sy = 1;

void Game::update()
{
  std::cout << "Game Updating.." << std::endl;

  // collisions 
  if (ball.x <= 0)
  {
    sx *= -1;
  }

  if (ball.x + ball.w >= screen_width)
  {
    sx *= -1;
  }

  if (ball.y <= 0)
  {
    sy *= -1;
  }

  if (ball.y + ball.h >= screen_height)
  {
    isRunning = false;
  }

  if (ball.y + ball.h >= paddle.y &&
      ball.x + ball.w >= paddle.x &&
      ball.x <= paddle.x + paddle.w)
  {
    sy *= -1.1;
    sx *= 1.1;
  }

  ball.x += sx;
  ball.y += sy;
  std::cout << "x:" << ball.x << " y:" << ball.y << std::endl;

}

void Game::render()
{
  std::cout << "Game Rendering..." << std::endl;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
  SDL_RenderClear(renderer);
  // actually render stuff

  SDL_SetRenderDrawColor(renderer, 255, 255 ,255, 1);
  SDL_RenderFillRect(renderer, &ball);
  SDL_RenderFillRect(renderer, &paddle);

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
