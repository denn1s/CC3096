#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <SDL2/SDL.h>
#include <box2d/box2d.h>

#include "Scene.h"
#include "Events.hpp"

#include "Entities.hpp"
#include "Components.hpp"
#include "System.h"

Scene::Scene(const std::string& name)
{
  std::cout << "Scene " << name << " constructed!" << std::endl;

  collisionEvent = SDL_RegisterEvents(1);

  world = new b2World({ 0.0f, 9.8f * 100 });
  world->SetContactListener(new ContactListener(collisionEvent));
}

Scene::~Scene()
{
  std::cout << "Scene Destroyed!" << std::endl;
}

Entity Scene::createEntity(const std::string& name, int x, int y)
{
  Entity entity = { r.create(), this };
  entity.addComponent<TagComponent>(name);
  entity.addComponent<TransformComponent>(x, y);

  return entity;
}

Entity Scene::createEntity(const std::string& name, b2BodyType type, int x, int y)
{
  Entity entity = { r.create(), this };
  entity.addComponent<TagComponent>(name);
  entity.addComponent<TransformComponent>(x, y);

  b2BodyDef bodyDef;
  bodyDef.type = type;
  bodyDef.position.Set(x, y);
  b2Body* body = world->CreateBody(&bodyDef);
  body->SetFixedRotation(true);

  struct BodyUserData
  {
      BodyUserData(Entity e) : entity(e) {};
      Entity entity;
  };

  BodyUserData* data = new BodyUserData(entity);
  body->GetUserData().pointer = reinterpret_cast<uintptr_t>(data);

  entity.addComponent<RigidBodyComponent>(type, body);

  return entity;
}

void Scene::addSetupSystem(SetupSystem* s)
{
  s->setScene(this);
  setupSystems.push_back(s);
}

void Scene::addInputSystem(InputSystem* s)
{
  s->setScene(this);
  inputSystems.push_back(s);
}

void Scene::addUpdateSystem(UpdateSystem* s)
{
  s->setScene(this);
  updateSystems.push_back(s);
}

void Scene::addRenderSystem(RenderSystem* s)
{
  s->setScene(this);
  renderSystems.push_back(s);
}

void Scene::setup()
{
  std::cout << "Scene Setup" << std::endl;

  for (SetupSystem* sys: setupSystems)
  {
    sys->run();
  }
}

void Scene::input(SDL_Event event)
{  
  for (InputSystem* sys: inputSystems)
  {
    sys->run(event);
  }
}

void Scene::update(double dT)
{
  std::cout << "Scene Update" << std::endl;
  
  for (UpdateSystem* sys: updateSystems)
  {
    sys->run(dT);
  }
}

void Scene::render(SDL_Renderer* renderer)
{
  std::cout << "Scene Render" << std::endl;
  
  for (RenderSystem* sys: renderSystems)
  {
    sys->run(renderer);
  }
}
