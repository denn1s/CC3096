#include <iostream>
#include <cmath>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Game.h"
#include "STexture.cpp"

int screen_width;
int screen_height;

SDL_Rect ball;
SDL_Rect paddle;
STexture* sampleTexture;

/*

  void SomeFunction ()
  {
    Enemy enemy;

    enemy.jump():
    enemy.run(10);
    enemy.look("LEFT");
  }

  Cada vez que creamos un objeto (sin la palabra new), el sistema operativo lo pone en un stack
  los bloques de memoria son continuos
  el tamaño de la memoria a alocar es conocido por el compilador
  el stack tiene un tamaño fijo (linux 2MB)
  no nos debemos peocupar por apartar la memoria
  cuando salimos del scope, se debe liberar la memoria, c++ saca el objeto del stack automaticamente
  c++ no tiene un garbage collector
  stack overflow
  esto ya lo hacemols, por ejemplo en el Game, podemos ver como llama al constructor y al destructor
  el stack no es el unico lugar en el que podemos almacenar cosas
  los sistemas operativos proveen un espacio mas grande para trabajar que se llama el heap


  void SomeFunction ()
  {
    Enemy* enemy = new Enemy(); // constructor method

    enemy->jump():  
    enemy->run(10);
    enemy->look("LEFT");

    delete enemy;  // we need to manually delete, esto llama al destructor
  }

  en el heap, la memoria se aparta dinamicamente.
  no tiene restricciones de memoria, podemos manejar toda la memoria del sistema
  es mas lento que el stack
  el stack es continuo, al heap hay que referenciarlo

  porque existe esto
    c -> malloc + constructor
    c -> destructor + free

    c++ -> new
    c++ -> delete

  */




Game::Game()
{
  FPS = 60;
  frameDuration = (1.0f / FPS) * 1000.0f;  // how many mili seconds in one frame
  std::cout << "Game Object Constructed!" << std::endl;
}

Game::~Game()
{
  std::cout << "Game Object Destroyed!" << std::endl;
}

void Game::init(const char* title, int width, int height)
{
  if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
  {
    window = SDL_CreateWindow(title, 0, 0, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    IMG_Init(IMG_INIT_PNG);
    std::cout << "Game Start!" << std::endl;

    screen_width = width;
    screen_height = height;

    isRunning = true;
  } else {
    isRunning = false;
  }
  counter = 0;
}

Uint32 fragment(Uint32 currentColor)
{
  return currentColor;
}

void Game::setup()
{
  ball.x = 20;
  ball.y = 20;
  ball.w = 300;
  ball.h = 300;

  sampleTexture = new STexture(renderer, window);
  sampleTexture->load("./src/Game/chr1.png");
  sampleTexture->applyShader(fragment);
}

void Game::frameStart()
{
  std::cout << "---- Frame: " << counter << " ----" << std::endl;
  frameStartTimestamp = SDL_GetTicks();
  if (frameEndTimestamp)
  {
    dT = (frameStartTimestamp - frameEndTimestamp) / 1000.0f;
  }
  else
  {
    dT = 0;
  }
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

int sx = 100;
int sy = 100;

void Game::update()
{
  std::cout << "Game Updating.." << std::endl;
}

void Game::render()
{
  std::cout << "Game Rendering..." << std::endl;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
  SDL_RenderClear(renderer);
  // actually render stuff

  SDL_SetRenderDrawColor(renderer, 255, 255 ,255, 1);


  // SDL_Surface* surface = IMG_Load("./src/Game/1.png");  
  // SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  sampleTexture->render(
    0,
    0,
    300,
    300
  );

  // SDL_FreeSurface(surface);
  // SDL_RenderCopy(renderer, texture, NULL, &ball);
  // SDL_DestroyTexture(texture);
  SDL_RenderFillRect(renderer, &paddle);

  SDL_RenderPresent(renderer);

}

void Game::clean()
{
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
	IMG_Quit();
  SDL_Quit();
  std::cout << "Game Over." << std::endl;
}

bool Game::running()
{
  return isRunning;
}
