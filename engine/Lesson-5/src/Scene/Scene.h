#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <entt/entt.hpp>

class Entity;

class Scene {
  private:
    friend class Entity;

    typedef void (*SetupSystem)(entt::registry& reg);
    typedef void (*UpdateSystem)(entt::registry& reg, double dT);
    typedef void (*RenderSystem)(entt::registry& reg, SDL_Renderer* renderer);

    entt::registry r;
    std::vector<SetupSystem> setupSystems;
    std::vector<UpdateSystem> updateSystems;
    std::vector<RenderSystem> renderSystems;

  public:
    Scene(const std::string&);
    ~Scene();

    Entity createEntity(
      const std::string& name = "NO NAME",
      int x = 0,
      int y = 0
    );
    void addSetupSystem(SetupSystem s);
    void addUpdateSystem(UpdateSystem s);
    void addRenderSystem(RenderSystem s);

    void setup();
    void update(double dT);
    void render(SDL_Renderer* renderer);
};

#endif