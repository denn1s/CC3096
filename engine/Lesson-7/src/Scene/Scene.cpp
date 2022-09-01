#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
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

Entity Scene::createEntity(const std::string& name, int x, int y, int z)
{
  Entity entity = { r.create(), this };
  entity.addComponent<TagComponent>(name);
  entity.addComponent<TransformComponent>(
    glm::vec3(x, y, z),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 0.0f)
  );

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

Entity* Scene::getMainCamera()
{
  return mainCamera;
}

void Scene::setMainCamera(Entity camera)
{
  std::cout << "Set Main Camera" << std::endl;
  mainCamera = &camera;
  std::cout << "Main camera: " << &mainCamera << std::endl;
}

glm::mat4 Scene::getMainCameraViewProj()
{
  std::cout << "Main camera2: " << &mainCamera << std::endl;

  const auto view = r.view<CameraComponent>();
  for (const entt::entity e : view) {
    auto c = view.get<CameraComponent>(e);
    std::cout << "C: " << glm::to_string(c.projection) << std::endl;
  }
  
  /*
  std::cout << "A: " << glm::to_string(transformComponent.translate) << std::endl;
  
  glm::mat4 translation = glm::translate(glm::mat4(1.0f), transformComponent.translate);
  glm::mat4 rotation = glm::toMat4(glm::quat(transformComponent.rotate));
  glm::mat4 scaling = glm::scale(glm::mat4(1.0f), transformComponent.scale);
  glm::mat4 cameraTransform = translation * rotation * scaling;
  std::cout << "T: " << glm::to_string(cameraTransform) << std::endl;

  const glm::mat4 cameraView = glm::inverse(cameraTransform);
  std::cout << "V: " << glm::to_string(cameraView) << std::endl;

  const glm::mat4 cameraProjection = mainCamera->getComponent<CameraComponent>().projection;

  return cameraProjection * cameraView;
  */
  return glm::mat4(1);
}
