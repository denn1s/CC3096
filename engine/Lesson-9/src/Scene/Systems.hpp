#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <vector>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <glm/gtx/io.hpp>
#include "./STexture.cpp"
#include "./System.h"
#include "./Components.hpp"

class PlayerInputSystem : public InputSystem {
  public:
    void run(SDL_Event event) override {
      auto& playerMovement = scene->player->getComponent<MovementComponent>();

      int speed = 100;

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
            break;
          case SDLK_RIGHT:
            playerMovement.vx = 0;
            break;
          case SDLK_UP:
            playerMovement.vy = 0;
            break;
          case SDLK_DOWN:
            playerMovement.vy = 0;
            break;
        }
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
      
      const std::string spritesheets[1] = {
          "./assets/spritesheets/Character.png",
      };

      SDL_Texture* tilesets[1];
    public:
        SpriteSystem(SDL_Renderer* r, SDL_Window* w, int fps) : renderer(r), window(w), FPS(fps) {}

        ~SpriteSystem() {}

        void run() override {
          for(int i = 0; i < 1; i++) {
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
              int framesToUpdate = floor(adT/afps);

              std::cout << "framesToUpdate: " << framesToUpdate << std::endl;

              if (framesToUpdate > 0) {
                sprite.xIndex += framesToUpdate;
                sprite.xIndex %= sprite.animationFrames;
                sprite.lastUpdate = current;
              }

              std::cout << "inde: " << sprite.xIndex << std::endl;
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

            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderCopy(renderer, tilesets[sprite.sheetIndex], &src, &dst);                        
          }
        }
};

class MovementUpdateSystem : public UpdateSystem {
    public:
        void run(double dT) override {
          const auto view = scene->r.view<TransformComponent, MovementComponent>();
          for (const entt::entity e : view) {
            auto& pos = view.get<TransformComponent>(e);
            const auto vel = view.get<MovementComponent>(e);

            pos.x += vel.vx * dT;
            pos.y += vel.vy * dT;
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

#endif