#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <SDL2/SDL.h>

#include "Scene.h"

#include "Entities.hpp"
#include "Components.hpp"
#include "System.h"


Scene::Scene(const std::string& name)
{
  std::cout << "Scene " << name << " constructed!" << std::endl;
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

  Entity camera = createEntity();
  auto& cameraTransform = camera.getComponent<TransformComponent>();
  cameraTransform.x = 320 * 4;
  cameraTransform.y = 240 * 4;
  camera.addComponent<CameraComponent>(4.0f);
  mainCamera = new Entity(camera);

  for (SetupSystem* sys: setupSystems)
  {
    sys->run();
  }
}

void Scene::input(SDL_Event event)
{
  std::cout << "Scene Input" << std::endl;
  
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
