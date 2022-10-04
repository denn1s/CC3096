#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <vector>
#include <iostream>
#include <random>
#include <string>
#include <SDL2/SDL.h>
#include <glm/gtx/io.hpp>
#include <box2d/box2d.h>
#include "./STexture.cpp"
#include "./System.h"
#include "./Components.hpp"

class PlayerInputSystem : public InputSystem {
  public:
    void run(SDL_Event event) override {
      auto& playerMovement = scene->player->getComponent<MovementComponent>();
      auto& playerSprite = scene->player->getComponent<SpriteComponent>();

      int speed = 1000;

      if (event.type == SDL_KEYDOWN)
      {
        switch (event.key.keysym.sym) {
          case SDLK_LEFT:
            playerMovement.vx = -speed;
            break;
          case SDLK_RIGHT:
            playerMovement.vx = speed;
            break;
          case SDLK_UP:
            playerMovement.vy = -speed;
            break;
          case SDLK_DOWN:
            playerMovement.vy = speed;
            break;
        }
      }  
      if (event.type == SDL_KEYUP)
      {
        switch (event.key.keysym.sym) {
          case SDLK_LEFT:
            playerMovement.vx = 0;
            if (playerMovement.vy == 0) {
              playerSprite.yIndex = 2;
            }
            break;
          case SDLK_RIGHT:
            playerMovement.vx = 0;
            if (playerMovement.vy == 0) {
              playerSprite.yIndex = 3;
            }
            break;
          case SDLK_UP:
            playerMovement.vy = 0;
            if (playerMovement.vx == 0) {
              playerSprite.yIndex = 1;
            }
            break;
          case SDLK_DOWN:
            playerMovement.vy = 0;
            if (playerMovement.vx == 0) {
              playerSprite.yIndex = 0;
            }
            break;
        }
      }
      if (playerMovement.vx < 0) {
        playerSprite.yIndex = 7;
      }
      if (playerMovement.vx > 0) {
        playerSprite.yIndex = 6;
      }
      if (playerMovement.vy < 0) {
        playerSprite.yIndex = 5;
      }
      if (playerMovement.vy > 0) {
        playerSprite.yIndex = 4;
      }
    }
};

struct Terrain {
    int index;  // 0 water, 1 dirt, 2 grass
    int x;
    int y;
};

struct Tile {
    Terrain bottom{-1, 0, 0};
    Terrain top{-1, 0, 0};
};

bool sameTerrain(int x, int y, int w, int h, int currentTerrain, Uint32* pixels) {
    if (x < 0 || x >= w || y < 0 || y >= h) {
        return false;
    }

    int otherTerrain = ((int)(pixels[y * w + x] >> 16) & 0xff);

    if (otherTerrain >= currentTerrain) {
        return true;
    }
    return false;
}

std::map<int, std::pair<int, int>> m = {
    { 0, {0, 96} },
    { 1, {0, 80} },
    { 2, {48, 96} },
    { 3, {48, 80} },
    { 4, {16, 96} },
    { 5, {16, 80} },
    { 6, {32, 96} },
    { 7, {32, 80} },
    { 8, {0, 48} },
    { 9, {0, 64} },
    { 10, {48, 48} },
    { 11, {48, 64} },
    { 12, {16, 48} },
    { 13, {16, 64} },
    { 14, {32, 48} },
    { 16, {80, 80} },
    { 17, {64, 80} },
    { 18, {80, 64} },
    { 19, {64, 64} },
};

void autotile(STexture *t, int x, int y, Tile* tilemaptile) {
    Uint32* pixels = t->getPixels();
    int w = t->getWidth();
    int h = t->getHeight();
    int currentTerrain = ((int)(pixels[y * w + x] >> 16) & 0xff);
    
    if (currentTerrain == 0) {
        tilemaptile->bottom = { 0, 0, 0 };
        return;
    } 

    int otherTerrain = currentTerrain - 1;
    int mask = 0;

    // North
    if (sameTerrain(x, y - 1, w, h, currentTerrain, pixels)) {
        mask += 1;
    }
    // West
    if (sameTerrain(x - 1, y, w, h, currentTerrain, pixels)) {
        mask += 2;
    }
    // East
    if (sameTerrain(x + 1, y, w, h, currentTerrain, pixels)) {
        mask += 4;
    }
    // South
    if (sameTerrain(x, y + 1, w, h, currentTerrain, pixels)) {
        mask += 8;
    }
    
    if (mask != 15) {
        tilemaptile->bottom = { otherTerrain, 0, 0 };
    } else {
        // NorthWest
        if (!sameTerrain(x - 1, y - 1, w, h, currentTerrain, pixels)) {
            mask = 16;
        }
        // NortEast
        if (!sameTerrain(x + 1, y - 1, w, h, currentTerrain, pixels)) {
            mask = 17;
        }
        // SouthWest
        if (!sameTerrain(x - 1, y + 1, w, h, currentTerrain, pixels)) {
            mask = 18;
        }
        // SouthEast
        if (!sameTerrain(x + 1, y + 1, w, h, currentTerrain, pixels)) {
            mask = 19;
        }

        if (mask == 15) {
            tilemaptile->bottom = { currentTerrain, 0, 0 };
        } else {
            tilemaptile->bottom = { otherTerrain, 0, 0 };
        }
    }

    auto transitionTile = m[mask];
    tilemaptile->top = { currentTerrain, transitionTile.first, transitionTile.second };
}

class AutoTileSystem : public SetupSystem, public RenderSystem {
    private:
        SDL_Renderer* renderer;
        SDL_Window* window;
        
        const static int srcTileSize = 16;

        const std::string mapfile = "./assets/tilemaps/6.png";        
        const std::string layerfiles[3] = {
            "./assets/tilesets/Water.png",
            "./assets/tilesets/Dirt.png",
            "./assets/tilesets/Grass.png",
        };

        SDL_Texture* tilesets[3];
        Tile* tilemap;
        int tilemapWidth;
        int tilemapHeight;

    public:
        AutoTileSystem(SDL_Renderer* r, SDL_Window* w) : renderer(r), window(w) {}

        ~AutoTileSystem() {}

        void run() override {
          for(int i = 0; i < 3; i++) {
            SDL_Surface* surface = IMG_Load(layerfiles[i].c_str());
            tilesets[i] = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
          }

          STexture* mapImage = new STexture(renderer, window);
          mapImage->load(mapfile);
          tilemapWidth = mapImage->getWidth();
          tilemapHeight = mapImage->getHeight();
          tilemap = new Tile[tilemapWidth * tilemapHeight];

          mapImage->lockTexture();
          for(int y = 0; y < tilemapHeight; y++) {
            for(int x = 0; x < tilemapWidth; x++) {
              autotile(mapImage, x, y, &tilemap[y * tilemapWidth + x]);
            }
          }
          mapImage->unlockTexture();

          delete mapImage;
        }

        void run(SDL_Renderer* r) override {
          auto cameraTransform = RenderSystem::scene->mainCamera->getComponent<TransformComponent>();
          auto cameraZoom = RenderSystem::scene->mainCamera->getComponent<CameraComponent>().zoom;

          const int dstTileSize = cameraZoom * srcTileSize;
          const int cx = cameraTransform.x;
          const int cy = cameraTransform.y;

          SDL_Rect dst = { -cx, -cy, dstTileSize, dstTileSize };

          for(int y = 0; y < tilemapHeight; y++) {
            for(int x = 0; x < tilemapWidth; x++) {
              Tile t = tilemap[y * tilemapWidth + x];
              if (t.bottom.index > -1) {
                SDL_Rect src = { t.bottom.x, t.bottom.y, srcTileSize, srcTileSize };
                SDL_RenderCopy(r, tilesets[t.bottom.index], &src, &dst);                        
              }
              if (t.top.index > -1) {
                SDL_Rect src = { t.top.x, t.top.y, srcTileSize, srcTileSize };
                SDL_RenderCopy(r, tilesets[t.top.index], &src, &dst);                        
              }
              dst.x += dstTileSize;
            }
            dst.x = -cx;
            dst.y += dstTileSize;
          }
        } 
};



class CharacterSetupSystem : public SetupSystem {
    public:
        CharacterSetupSystem() {}

        ~CharacterSetupSystem() {}

        void run() override {
          auto cameraComponent = scene->mainCamera->getComponent<CameraComponent>();

          Entity player = scene->createEntity(
            "PLAYER",
            b2_dynamicBody,
            cameraComponent.ww / 2 + cameraComponent.vw / 2 - 12 * cameraComponent.zoom,
            cameraComponent.wh / 2 + cameraComponent.vh / 2 - 12 * cameraComponent.zoom
          );
          player.addComponent<MovementComponent>(0, 0);
          player.addComponent<SpriteComponent>(
            0, // sheetIndex
            8, // total animationFrames
            5, // animation duration seconds
            0, // x index for this sprite
            0, // y index for this sprite
            48 // src size for the sprite
          );
          /*
          player.addComponent<BoxComponent>(
            48, // x offset of the box
            48, // y offset of the box
            24, // width of the box
            24, // height of the box
            SDL_Color{0, 0, 200}
          );
          */
          player.addComponent<BoxColliderComponent>(
            16*cameraComponent.zoom, // x offset of the box
            16*cameraComponent.zoom, // y offset of the box
            16, // width of the box
            16, // height of the box
            1.0f, // density
            0.0f // friction
          );
          
          scene->player = new Entity(player);
        }
};

class CameraSetupSystem : public SetupSystem {
    private:
      int viewportWidth;
      int viewportHeight;
      int worldWidth;
      int worldHeight;
      int zoom;

    public:
        CameraSetupSystem(int z, int vw, int vh, int ww, int wh) : 
          zoom(z),
          viewportWidth(vw),
          viewportHeight(vh),
          worldWidth(ww),
          worldHeight(wh)
        {}

        void run() override {
          Entity camera = scene->createEntity("CAMERA",
            (worldWidth) / 2,
            (worldHeight) / 2 
          );
          camera.addComponent<CameraComponent>(
            zoom,
            viewportWidth,
            viewportHeight,
            worldWidth,
            worldHeight
          );
          scene->mainCamera = new Entity(camera);
        }
};

class SpriteSystem : public SetupSystem, public UpdateSystem, public RenderSystem {
  private:
      SDL_Renderer* renderer;
      SDL_Window* window;
      int FPS;
      
      const std::string spritesheets[2] = {
          "./assets/spritesheets/Character.png",
          "./assets/spritesheets/Bugs.png"
      };

      SDL_Texture* tilesets[2];
    public:
        SpriteSystem(SDL_Renderer* r, SDL_Window* w, int fps) : renderer(r), window(w), FPS(fps) {}

        ~SpriteSystem() {}

        void run() override {
          for(int i = 0; i < 2; i++) {
            SDL_Surface* surface = IMG_Load(spritesheets[i].c_str());
            tilesets[i] = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
          }
        }

        void run(double dT) override {
          const auto view = UpdateSystem::scene->r.view<SpriteComponent>();

          Uint32 current = SDL_GetTicks();

          for (const entt::entity e : view) {
            auto& sprite = view.get<SpriteComponent>(e);

            if (sprite.animationDurationSeconds > 0) {
              float adT = (current - sprite.lastUpdate) / 1000.0f;
              float afps = sprite.animationDurationSeconds/FPS;
              int framesToUpdate = adT/afps;

              if (framesToUpdate > 0) {
                sprite.xIndex += framesToUpdate;
                sprite.xIndex %= sprite.animationFrames;
                sprite.lastUpdate = current;
              }
            }
          }
        }

        void run(SDL_Renderer* renderer) override {
          auto cameraTransform = RenderSystem::scene->mainCamera->getComponent<TransformComponent>();
          auto cameraZoom = RenderSystem::scene->mainCamera->getComponent<CameraComponent>().zoom;
          const int cx = cameraTransform.x;
          const int cy = cameraTransform.y;

          const auto view = RenderSystem::scene->r.view<TransformComponent, SpriteComponent>();
          for (const entt::entity e : view) {
            const auto pos = view.get<TransformComponent>(e);
            const auto sprite = view.get<SpriteComponent>(e);

            const int dstTileSize = cameraZoom * sprite.size;
            const int spriteX = sprite.xIndex * sprite.size;
            const int spriteY = sprite.yIndex * sprite.size;

            SDL_Rect src = { spriteX, spriteY, sprite.size, sprite.size };
            SDL_Rect dst = { pos.x - cx, pos.y - cy, dstTileSize, dstTileSize };

            SDL_RenderCopyEx(renderer, tilesets[sprite.sheetIndex], &src, &dst, sprite.rotate, NULL, SDL_FLIP_NONE);
          }
        }
};

class MovementUpdateSystem : public UpdateSystem {
    public:
        void run(double dT) override {
          const auto view = scene->r.view<RigidBodyComponent, MovementComponent>();
          for (const entt::entity e : view) {
            const auto rb = view.get<RigidBodyComponent>(e);
            const auto vel = view.get<MovementComponent>(e);
            b2Vec2 prevVel = rb.body->GetLinearVelocity();

            rb.body->SetLinearVelocity(
              b2Vec2{
                (float)vel.vx, 
                prevVel.y
              });

            if (vel.vy > 0) {
              std::cout << "jump!" << std::endl;
              rb.body->ApplyLinearImpulse(
                b2Vec2{0.0f, -100.0f * rb.body->GetMass()},
                rb.body->GetPosition(),
                true
              );
            } else {
              
            }
          }
        }
};

class MovementPhysicsUpdateSystem : public UpdateSystem {
    public:
        void run(double dT) override {
          const int velocityIterations = 6;
          const int positionIterations = 2;
          std::cout << "dT: " << dT << std::endl;

          scene->world->Step(dT, velocityIterations, positionIterations);

          const auto view = scene->r.view<TransformComponent, RigidBodyComponent>();
          
          for (const entt::entity e : view) {
            Entity en = { e, scene };

            auto& pos = view.get<TransformComponent>(e);
            const auto rb = view.get<RigidBodyComponent>(e);
            const auto bodyPos = rb.body->GetPosition();

            pos.x = bodyPos.x;
            pos.y = bodyPos.y;

            std::cout << en.getComponent<TagComponent>().tag << std::endl;
            std::cout << "bodyPos.x: " << bodyPos.x << std::endl;
            std::cout << "bodyPos.y: " << bodyPos.y << std::endl << std::endl;
          }
        }
};

class CameraFollowUpdateSystem : public UpdateSystem {
    public:
        void run(double dT) override {
          const int spriteSize = 48;
          auto playerTransform = scene->player->getComponent<TransformComponent>();
          auto cameraComponent = scene->mainCamera->getComponent<CameraComponent>();
          auto& cameraTransform = scene->mainCamera->getComponent<TransformComponent>();

          int px = playerTransform.x - cameraComponent.vw / 2 + (spriteSize / 2) * cameraComponent.zoom;
          int py = playerTransform.y - cameraComponent.vh / 2 + (spriteSize / 2) * cameraComponent.zoom;

          if (px > 0 && px < cameraComponent.ww - cameraComponent.vw) {
            cameraTransform.x = playerTransform.x - cameraComponent.vw / 2 + (spriteSize / 2) * cameraComponent.zoom;
          }

          if (py > 0 && py < cameraComponent.wh - cameraComponent.vh) {
            cameraTransform.y = playerTransform.y - cameraComponent.vh / 2 + (spriteSize / 2) * cameraComponent.zoom;
          }
        }
};

class EnemySpawnSystem : public SetupSystem {
    public:
        EnemySpawnSystem() {}

        ~EnemySpawnSystem() {}

        void run() override {
          auto cameraTransform = scene->mainCamera->getComponent<TransformComponent>();
          auto cameraComponent = scene->mainCamera->getComponent<CameraComponent>();

          std::random_device rd;
          std::mt19937 gen(rd());
          
          std::uniform_int_distribution<> random_x(cameraTransform.x, cameraTransform.x + cameraComponent.vw);
          std::uniform_int_distribution<> random_y(cameraTransform.y, cameraTransform.y + cameraComponent.vh);

          
          Entity enemy = scene->createEntity(
            "ENEMY",
            (int)random_x(gen),
            (int)random_y(gen)
          );

          int enemyIndex = 0;

          enemy.addComponent<EnemyComponent>(enemyIndex, 0);
          enemy.addComponent<MovementComponent>(0, 0);
          enemy.addComponent<SpriteComponent>(
            1, // sheetIndex
            2, // total animationFrames
            0, // animation duration seconds
            0, // x index for this sprite
            enemyIndex * 16, // y index for this sprite
            16 // src size for the sprite
          );
          /*
          enemy.addComponent<BoxComponent>(
            0, // x offset of the box
            0, // y offset of the box
            16, // width of the box
            16, // height of the box
            SDL_Color{0, 0, 0}
          );
          */ 
        }
};

class EnemyMovementSystem : public UpdateSystem {
    public:
        EnemyMovementSystem() {}

        ~EnemyMovementSystem() {}

        void run(double dT) override {
          auto playerTransform = scene->player->getComponent<TransformComponent>();

          const auto view = scene->r.view<
            EnemyComponent,
            TransformComponent,
            SpriteComponent,
            MovementComponent
          >();

          for (const entt::entity e : view) {
            const auto enemy = view.get<EnemyComponent>(e);
            const auto pos = view.get<TransformComponent>(e);
            auto& sprite = view.get<SpriteComponent>(e);
            auto& vel = view.get<MovementComponent>(e);

            int tx = playerTransform.x + 48;
            int ty = playerTransform.y + 48;

            switch(enemy.aiIndex) {
              case 0:
                int speed = 100;
                if (ty > pos.y) {
                  vel.vx = 0;
                  vel.vy = speed;
                } else if (ty < pos.y) {
                  vel.vx = 0;
                  vel.vy = -speed;
                } else if (tx > pos.x) {
                  vel.vx = speed;
                  vel.vy = 0;
                } else if (tx < pos.x) {
                  vel.vx = -speed;
                  vel.vy = 0;
                } else {
                  vel.vx = 0;
                  vel.vy = 0;
                }
                if (vel.vx > 0) {
                  sprite.rotate = 90;
                }
                if (vel.vx < 0) {
                  sprite.rotate = -90;
                }
                if (vel.vy < 0) {
                  sprite.rotate = 0;
                }
                if (vel.vy > 0) {
                  sprite.rotate = 180;
                }

                if (vel.vx == 0 && vel.vy == 0) {
                  sprite.animationDurationSeconds = 0;
                } else {
                  sprite.animationDurationSeconds = 1;
                }
                break;
            }
          }
        }
};

class BoxColliderRenderSystem : public RenderSystem {
    public:
      void run(SDL_Renderer* r) override {
        const auto cameraZoom = scene->mainCamera->getComponent<CameraComponent>().zoom;
        const auto cameraTransform = scene->mainCamera->getComponent<TransformComponent>();
        const int cx = cameraTransform.x;
        const int cy = cameraTransform.y;

        const auto view = scene->r.view<RigidBodyComponent, BoxColliderComponent>();

        for (const entt::entity e : view) {
          const auto box = view.get<BoxColliderComponent>(e);
          const auto rb = view.get<RigidBodyComponent>(e);
          const auto pos = rb.body->GetPosition();
          
          //for (const auto fixture : rb.body->GetFixtureList()) {
            SDL_SetRenderDrawColor(r, box.color.r, box.color.g, box.color.b, 255);
            
            SDL_Rect rect = {
              (int)pos.x - cx + box.xo, 
              (int)pos.y - cy + box.yo,
              box.w * cameraZoom,
              box.h * cameraZoom
            };

            SDL_RenderDrawRect(r, &rect);
          //}
        }
      }
};

/*

class BoxRenderSystem : public RenderSystem {
    public:
      void run(SDL_Renderer* r) override {
        const auto cameraTransform = scene->mainCamera->getComponent<TransformComponent>();
        const auto cameraComponent = scene->mainCamera->getComponent<CameraComponent>();
        const int cx = cameraTransform.x;
        const int cy = cameraTransform.y;

        const auto view = scene->r.view<RigidBodyComponent, BoxColliderComponent, TransformComponent>();

        for (const entt::entity e : view) {
          const auto rb = view.get<BoxColliderComponent>(e);
          const auto pos = view.get<TransformComponent>(e);

          SDL_SetRenderDrawColor(r, box.color.r, box.color.g, box.color.b, 255);
          
          SDL_Rect rect = {
            pos.x - cx + box.xo, 
            pos.y - cy + box.xo,
            box.w * cameraComponent.zoom,
            box.h * cameraComponent.zoom
          };


          if (box.fill) {
            SDL_RenderFillRect(r, &rect);
          } else {
            SDL_RenderDrawRect(r, &rect);
          }

        }
      }
};
*/


class BoxColliderSetupSystem : public SetupSystem {
    public:
      void run() override {
        const auto cameraZoom = scene->mainCamera->getComponent<CameraComponent>().zoom;
        const auto view = scene->r.view<TransformComponent, RigidBodyComponent, BoxColliderComponent>();

        for (const entt::entity e : view) {
          auto& rb = view.get<RigidBodyComponent>(e);
          const auto pos = view.get<TransformComponent>(e);
          const auto collider = view.get<BoxColliderComponent>(e);
          const float hw = (collider.w/2.0f * cameraZoom);
          const float hh = (collider.h/2.0f * cameraZoom);

          b2PolygonShape box;
          box.SetAsBox(
            hw,
            hh,
            b2Vec2{collider.xo + hw, collider.yo + hh},
            0.0f
          );
          
          b2FixtureDef fixtureDef;
          fixtureDef.shape = &box;
          fixtureDef.density = collider.density;
          fixtureDef.friction = collider.friction;
          fixtureDef.restitution = collider.restitution;
          fixtureDef.restitutionThreshold = collider.restitutionThreshold;


          rb.body->CreateFixture(&fixtureDef);  
        }
      }
};

class PhysicsDemoSystem : public SetupSystem {
    public:
      void run() override {
        const auto cameraTransform = scene->mainCamera->getComponent<TransformComponent>();

        int floorX = cameraTransform.x + 100;
        int floorY = cameraTransform.y + 400;
        int floorW = 140;
        int floorH = 10;

        Entity floor = scene->createEntity(
          "FLOOR",
          b2_staticBody,
          floorX,
          floorY
        );

        floor.addComponent<BoxColliderComponent>(
            0, // x offset of the box
            0, // y offset of the box
            floorW, // width of the box
            floorH, // height of the box
            1.0f // density
          );
      }
};

#endif