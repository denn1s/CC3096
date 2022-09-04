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

Entity Scene::createEntity(const std::string& name, int x, int y, int z)
{
  Entity entity = { r.create(), this };
  entity.addComponent<TagComponent>(name);
  entity.addComponent<TransformComponent>(
    glm::vec3(x, y, z)
    // glm::vec3(1.0f, 1.0f, 1.0f),
    // glm::vec3(0.0f, 0.0f, 0.0f)
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

  Entity camera = createEntity();
  camera.addComponent<CameraComponent>(CameraComponent{glm::mat4(1.0f)});
  mainCamera = new Entity(camera);

  for (SetupSystem* sys: setupSystems)
  {
    sys->run();
  }
}

void Scene::update(double dT)
{
  std::cout << "Scene Update" << std::endl;
  updateCameraTransform();
  
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

void Scene::updateCameraTransform()
{
  const int screenWidth = 800;
  const int screenHeight = 600;

  // auto cameraComponent = mainCamera->getComponent<CameraComponent>();
  auto& transformComponent = mainCamera->getComponent<TransformComponent>();

  transformComponent.translate.x += 1;

  /*
  glm::mat4 translation = glm::translate(glm::mat4(1.0f), transformComponent.translate);
  glm::mat4 rotation = glm::toMat4(glm::quat(transformComponent.rotate));
  glm::mat4 scaling = glm::scale(glm::mat4(1.0f), transformComponent.scale);
  glm::mat4 cameraTransform = translation * rotation * scaling;

  glm::mat4 cameraView = glm::lookAt(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(0,1,0));
  glm::mat4 cameraProjection = glm::ortho(
        -(screenWidth/2.0f), (screenWidth/2.0f), 
         (screenHeight/2.0f), -(screenHeight/2.0f), 
        -10.0f, 10.0f);

  glm::mat4x4 cameraViewport{
      (screenWidth/2), 0, 0, (screenWidth/2),
      0, (screenHeight/2), 0, (screenHeight/2),
      0, 0, 128, 128,
      0, 0, 0, 1
  };

  mainCameraTransform = cameraViewport * cameraProjection * cameraView * cameraTransform;

  std::cout << "Transforme: " << mainCameraTransform << std::endl;
  */
  mainCameraTransform = transformComponent.translate;
}

glm::vec3 Scene::getCameraTransform()
{
  return mainCameraTransform;
}