#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>

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
  int flip = 0;
};

struct EnemyComponent {
  int enemyIndex = 0;
  int aiIndex = 0;
};

#endif