#include <iostream>

#include <entt/entt.hpp>

class Entity
{
  public:
    Entity();
    ~Entity();

    bool hasComponent();

  private:
    entt::entity handle;
    Scene* scene;
}

Entity::Entity()
{
  std::cout << "Entity" << std::endl;
}

Entity::hasComponent()
{
  scene->registry
}