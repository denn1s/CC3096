#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "Scene.h"

#include "Entity.hpp"
#include "Components.hpp"


Scene::Scene(const std::string& name)
{
  std::cout << "Scene " << name << "constructed!" << std::endl;
}

Scene::~Scene()
{
  std::cout << "Scene Destroyed!" << std::endl;
}

Entity Scene::createEntity(const std::string& name, int x, int y)
{
  Entity entity = { r.create(), this };
  entity.addComponent<TagComponent>(TagComponent{name});
  entity.addComponent<TransformComponent>(
    TransformComponent{glm::vec2(x, y),glm::vec2(1, 1),0.0d}
  );

  return entity;
}

void Scene::addSetupSystem(SetupSystem s)
{
  setupSystems.push_back(s);
}

void Scene::addUpdateSystem(UpdateSystem s)
{
  updateSystems.push_back(s);
}

void Scene::addRenderSystem(RenderSystem s)
{
  renderSystems.push_back(s);
}

void Scene::setup()
{
  std::cout << "Scene Setup" << std::endl;
  
  for (SetupSystem &sys: setupSystems)
  {
    sys(r);
  }
}

void Scene::update(double dT)
{
  std::cout << "Scene Update" << std::endl;
  
  for (UpdateSystem &sys: updateSystems)
  {
    sys(r, dT);
  }
}

void Scene::render(SDL_Renderer* renderer)
{
  std::cout << "Scene Render" << std::endl;
  
  for (RenderSystem &sys: renderSystems)
  {
    sys(r, renderer);
  }
}