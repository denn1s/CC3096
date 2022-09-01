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

  glm::mat4 getTransform()
  {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), translate);
    glm::mat4 rotation = glm::toMat4(glm::quat(rotate));
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);

    return translation * rotation * scaling;
  }
};

struct CameraComponent {
  glm::mat4 projection = glm::mat4(1.0f);
};

struct MovementComponent {
  glm::vec2 velocity;
};

struct ColliderComponent {
  glm::vec2 size;
};

#endif