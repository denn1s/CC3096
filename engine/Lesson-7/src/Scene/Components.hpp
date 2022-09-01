#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>

struct TagComponent {
  std::string tag;
};

struct TransformComponent {
  glm::vec3 translate = { 0.0f, 0.0f, 0.0f };
  glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
  glm::vec3 rotate = { 0.0f, 0.0f, 0.0f };
};

struct CameraComponent {
  glm::mat4 projection = glm::mat4(1.0f);
  bool isMain = false;
};

struct MovementComponent {
  glm::vec2 velocity;
};

struct ColliderComponent {
  glm::vec2 size;
};

#endif