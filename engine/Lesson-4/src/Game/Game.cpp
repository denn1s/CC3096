#include <iostream>
#include <cmath>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <entt/entt.hpp>
#include "../Scene/Scene.h"
#include "Game.h"
#include "STexture.cpp"

int screen_width;
int screen_height;

entt::registry reg;

struct PositionComponent {
  int x, y;
};

struct VelocityComponent {
  int x, y;
};

struct CubeComponent {
  int w, h;
};

PositionComponent playerSpawnPosition = {20, 20};
VelocityComponent playerSpawnVelocity = {200, 200};
CubeComponent playerRect = {100, 100};

void createPlayerEntity()
{
  const entt::entity e = reg.create();
  reg.emplace<PositionComponent>(e, playerSpawnPosition);
  reg.emplace<VelocityComponent>(e, playerSpawnVelocity);
  reg.emplace<CubeComponent>(e, playerRect);  
}

void cubeRenderSystem(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255 ,255, 1);

  const auto view = reg.view<PositionComponent, CubeComponent>();
  for (const entt::entity e : view) {
    const PositionComponent position = view.get<PositionComponent>(e);
    const CubeComponent cube = view.get<CubeComponent>(e);

    SDL_Rect rect = { position.x, position.y, cube.w, cube.h };    
    SDL_RenderFillRect(renderer, &rect);
  }
}

void movementSystem(double dT) {
  auto view = reg.view<PositionComponent, VelocityComponent, CubeComponent>();
  for (const entt::entity e : view) {
    PositionComponent& pos = view.get<PositionComponent>(e);
    VelocityComponent& vel = view.get<VelocityComponent>(e);
    const CubeComponent& cub = view.get<CubeComponent>(e);

    // collisions 
    if (pos.x <= 0)
    {
      vel.x *= -1;
    }

    if (pos.x + cub.w >= screen_width)
    {
      vel.x *= -1;
    }

    if (pos.y <= 0)
    {
      vel.y *= -1;
    }

    if (pos.y + cub.h >= 480)
    {
      vel.y *= -1;
    }

    pos.x += vel.x * dT;
    pos.y += vel.y * dT;
  }
}

bool bounceInputSystem(int key) {
  if (key != SDLK_SPACE) {
    return false;
  }

  auto view = reg.view<VelocityComponent>();
  for (const entt::entity e : view) {
    VelocityComponent& vel = view.get<VelocityComponent>(e);
    vel.x *= -1.2;
    vel.y *= -1.2;
  }
  return true;
}

Game::Game()
{
  FPS = 60;
  frameDuration = (1.0f / FPS) * 1000.0f;  // how many mili seconds in one frame
  std::cout << "Game Object Constructed!" << std::endl;

  SDL_RenderSetLogicalSize(renderer, 160, 144);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
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

void Game::setup()
{
  createPlayerEntity();
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
      bounceInputSystem(event.key.keysym.sym);
    }
  }
}

void Game::update()
{
  std::cout << "Game Updating.." << std::endl;

  movementSystem(dT);
}

// SDL_Rect s2 = { 250, 0, 240, 240 };

void Game::render()
{
  std::cout << "Game Rendering..." << std::endl;

  SDL_SetRenderDrawColor(renderer, 0, 0 , 0, 1);

  SDL_RenderClear(renderer);

  // SDL_RenderCopy(renderer, newTexture, NULL, &s2);

  cubeRenderSystem(renderer);

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
