#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <entt/entt.hpp>
#include <box2d/box2d.h>

class Entity;
class SetupSystem;
class InputSystem;
class UpdateSystem;
class RenderSystem;

class Scene {
  private:
    std::vector<SetupSystem*> setupSystems;
    std::vector<InputSystem*> inputSystems;
    std::vector<UpdateSystem*> updateSystems;
    std::vector<RenderSystem*> renderSystems;

  public:
    Scene(const std::string&);
    ~Scene();

    entt::registry r;
    Entity* mainCamera;
    Entity* player;
    b2World* world;
    Uint32 collisionEvent;

    Entity createEntity(
      const std::string& name = "NO NAME",
      int x = 0,
      int y = 0
    );

    Entity createEntity(
      const std::string& name = "NO NAME",
      b2BodyType type = b2_staticBody,
      int x = 0,
      int y = 0
    );


    void addSetupSystem(SetupSystem* s);
    void addInputSystem(InputSystem* s);
    void addUpdateSystem(UpdateSystem* s);
    void addRenderSystem(RenderSystem* s);

    void setup();
    void update(double dT);
    void input(SDL_Event event);
    void render(SDL_Renderer* renderer);
};

#endif