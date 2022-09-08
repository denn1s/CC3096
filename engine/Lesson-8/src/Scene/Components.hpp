#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>

struct TagComponent {
  std::string tag;
};

struct TransformComponent {
    glm::vec2 translate = { 0.0f, 0.0f };
};

struct CameraComponent {
  float zoom = 1.0f;
};

struct SpriteComponent {
  int x = 0;
  int y = 0;
  int size = 24;
  SDL_Texture* texture;
};


struct MovementComponent {
  glm::vec2 velocity;
};

struct ColliderComponent {
  glm::vec2 size;
};

#endif