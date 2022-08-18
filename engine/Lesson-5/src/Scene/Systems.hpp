#include <iostream>
#include <SDL2/SDL.h>

#include "./System.h"
#include "./Components.hpp"

class MovementSystem : public UpdateSystem {
  private:
    int counter = 0;

  public:
    MovementSystem(int c) : counter(c) {}

    void run(double dT) override {
      const auto view = scene->r.view<TransformComponent, MovementComponent>();
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
    }
};

class CubeSystem : public RenderSystem {
  public:
    void run(SDL_Renderer* renderer) override {
      SDL_SetRenderDrawColor(renderer, 255, 100, 100, 1);

      const auto view = scene->r.view<TransformComponent, ColliderComponent>();
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
    }
};

class HelloSystem : public SetupSystem {
  public:
    HelloSystem() {
      std::cout << "Hello System Constructor" << std::endl;
    }

    ~HelloSystem() {
      std::cout << "Hello System Destructor" << std::endl;
    }

    void run() override {
      std::cout << "Hello System!" << std::endl;
    }
};
