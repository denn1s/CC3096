#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <box2d/box2d.h>

struct TagComponent {
  std::string tag;
};

struct TransformComponent {
  int x = 0;
  int y = 0;
};

struct MovementComponent {
  int vx = 0;
  int vy = 0;
};

struct CameraComponent {
  int zoom = 1;
  int vw = 0;
  int vh = 0;
  int ww = 0;
  int wh = 0;
};

struct SpriteComponent {
  int sheetIndex = 0;
  int animationFrames = 1;
  float animationDurationSeconds = 0;
  int xIndex = 0;
  int yIndex = 0;
  int size = 48;
  int lastUpdate = 0;
  int rotate = 0;
};

struct EnemyComponent {
  int enemyIndex = 0;
  int aiIndex = 0;
};

/*
struct BoxComponent {
  int xo = 0;
  int yo = 0;
  int w = 0;
  int h = 0;
  SDL_Color color = {255, 255, 255};
  bool fill = false;
};
*/

struct RigidBodyComponent {
  b2BodyType type = b2_staticBody;  // static, dynamc, kinematic
  b2Body* body = nullptr;
};

struct BoxColliderComponent {
  int xo = 0;
  int yo = 0;
  int w = 0;
  int h = 0;
  float density = 1.0f;
  float friction = 0.1f;
  float restitution = 0.0f;
  float restitutionThreshold = 0.0f;
  SDL_Color color = {255, 255, 255};
};

#endif