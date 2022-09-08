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

class CameraInputSystem : public InputSystem {
  public:
    void run(SDL_Event event) override {
      auto& cameraTransform = scene->mainCamera->getComponent<TransformComponent>().translate;
      auto& cameraZoom = scene->mainCamera->getComponent<CameraComponent>().zoom;

      if (event.type == SDL_KEYDOWN)
      {
        switch (event.key.keysym.sym) {
          case SDLK_LEFT:
            cameraTransform.x -= 10;
            break;
          case SDLK_RIGHT:
            cameraTransform.x += 10;
            break;
          case SDLK_UP:
            cameraTransform.y -= 10;
            break;
          case SDLK_DOWN:
            cameraTransform.y += 10;
            break;
          case SDLK_z:
            cameraZoom += 0.1;
            break;
          case SDLK_x:
            cameraZoom -= 0.1;
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
          auto cameraTransform = RenderSystem::scene->mainCamera->getComponent<TransformComponent>().translate;
          auto cameraZoom = RenderSystem::scene->mainCamera->getComponent<CameraComponent>().zoom;

          const int dstTileSize = cameraZoom * srcTileSize;
          const int cx = -cameraTransform.x;
          const int cy = -cameraTransform.y;

          SDL_Rect dst = { cx, cy, dstTileSize, dstTileSize };

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
            dst.x = cx;
            dst.y += dstTileSize;
          }
        } 
};



class CharacterSetupSystem : public SetupSystem {
    private:
        const std::string spritefile = "./assets/characters/fairy/front/1.png";        
        SDL_Renderer* renderer;

    public:
        CharacterSetupSystem(SDL_Renderer* r) : renderer(r) {}

        ~CharacterSetupSystem() {}

        void run() override {
          SDL_Surface* surface = IMG_Load(spritefile.c_str());
          SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
          SDL_FreeSurface(surface);


          Entity player = scene->createEntity("PLAYER", 100, 100);
          player.addComponent<SpriteComponent>(0, 0, 24, texture);
        }
};

class SpriteRenderSystem : public RenderSystem {
    public:
        SpriteRenderSystem() {}

        ~SpriteRenderSystem() {}

        void run(SDL_Renderer* renderer) override {
          auto cameraTransform = scene->mainCamera->getComponent<TransformComponent>().translate;
          auto cameraZoom = scene->mainCamera->getComponent<CameraComponent>().zoom;
          const int cx = cameraTransform.x;
          const int cy = cameraTransform.y;

          const auto view = scene->r.view<TransformComponent, SpriteComponent>();
          for (const entt::entity e : view) {
            const auto pos = view.get<TransformComponent>(e).translate;
            const auto sprite = view.get<SpriteComponent>(e);
            const int dstTileSize = cameraZoom * sprite.size;

            SDL_Rect src = { sprite.x, sprite.y, sprite.size, sprite.size };
            SDL_Rect dst = { pos.x, pos.y, dstTileSize, dstTileSize };

            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

            // SDL_RenderFillRect(renderer, &dst);
            SDL_RenderCopy(renderer, sprite.texture, &src, &dst);                        
          }
        }
};

#endif