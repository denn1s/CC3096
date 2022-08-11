#include <iostream>
#include "Scene.h"

Scene::Scene()
{
  std::cout << "Scene Constructed!" << std::endl;
}

Scene::~Scene()
{
  std::cout << "Scene Destroyed!" << std::endl;
}
