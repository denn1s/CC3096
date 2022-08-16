#include <iostream>
#include <SDL2/SDL.h>

#include "../Scene/Components.hpp"


int counter = 0;

void HelloSystem(entt::registry& reg) {
  std::cout << "IM A SYSTEM" << std::endl;
};

void MovementSystem(entt::registry& reg, double dT) {
  const auto view = reg.view<TransformComponent, MovementComponent>();
  for (const entt::entity e : view) {
    TransformComponent& t = view.get<TransformComponent>(e);
    MovementComponent& m = view.get<MovementComponent>(e);

    if (t.position.x <= 0)
    {
      m.velocity.x *= -1;
    }

    if (t.position.x >= 640)
    {
      m.velocity.x *= -1;
    }

    if (t.position.y <= 0)
    {
      m.velocity.y *= -1;
    }

    if (t.position.y >= 480)
    {
      m.velocity.y *= -1;
    }

    t.position.x += m.velocity.x * dT;
    t.position.y += m.velocity.y * dT;

    std::cout << "x: " << t.position.x << std::endl;
    std::cout << "y: " << t.position.y << std::endl;
    std::cout << "c: " << counter++ << std::endl;
  }
};

void CubeSystem(entt::registry& reg, SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255 ,255, 1);

  const auto view = reg.view<TransformComponent, ColliderComponent>();
  for (const entt::entity e : view) {
    const TransformComponent& t = view.get<TransformComponent>(e);
    const ColliderComponent& c = view.get<ColliderComponent>(e);
    const int x = t.position.x;
    const int y = t.position.y;
    const int w = c.size.x;
    const int h = c.size.y;

    SDL_Rect rect = { x, y, w, h };    
    SDL_RenderFillRect(renderer, &rect);
  }
};