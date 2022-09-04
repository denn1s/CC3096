#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <vector>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <glm/gtx/string_cast.hpp>
#include "./STexture.cpp"
#include "./System.h"
#include "./Components.hpp"

/*
class CameraSetupSystem : public SetupSystem {
  public:
    void run() override {
        Entity camera = scene->createEntity();
        camera.addComponent<CameraComponent>(CameraComponent{glm::mat4(1.0f)});
        std::cout << "Main Camera Created: " << &camera << std::endl;

        scene->setMainCamera(new Entity(camera));
    }
};
*/

struct Terrain {
  int index;
  int x;
  int y;
};

struct Tile {
  Terrain bottom{-1, 0, 0};
  Terrain top{-1, 0, 0};
};

bool sameTerrain(int x, int y, int w, int h, int currentTerrain, Uint32* pixels)
{
  if (x < 0 || x >= w || y < 0 || y > h) {
    return false;
  }
  if ((int)((pixels[(y * w) + x] >> 16) & 0xff) >= currentTerrain) {
    return true;
  }
  return false;
}

void autoTiling2(STexture* t, int x, int y, Tile* tilemaptile) {
  Uint32 currentColor = t->getPixel(x, y);
  int currentTerrain = ((int)(currentColor >> 16) & 0xff);
  int currentTile = ((int)(currentColor >> 8) & 0xff);
  int w = t->getWidth();
  int h = t->getHeight();
  Uint32* pixels = t->getPixels();

  int mask = 0;
  int otherTerrain = currentTerrain - 1;

  // North
  if (sameTerrain(x,  y - 1, w, h, currentTerrain, pixels)) {
    mask += 1;
  }

  // West
  if (sameTerrain(x - 1,  y, w, h, currentTerrain, pixels)) {
    mask += 2;
  }

  // East
  if (sameTerrain(x + 1,  y, w, h, currentTerrain, pixels)) {
    mask += 4;
  }

  // South
  if (sameTerrain(x, y + 1, w, h, currentTerrain, pixels)) {
    mask += 8;
  }

  if (mask == 15) {
    // North West
    if (!sameTerrain(x - 1,  y - 1, w, h, currentTerrain, pixels)) {
      mask = 16;
    }
    // North East
    if (!sameTerrain(x + 1,  y - 1, w, h, currentTerrain, pixels)) {
      mask = 17;
    }
    // South West
    if (!sameTerrain(x - 1,  y + 1, w, h, currentTerrain, pixels)) {
      mask = 18;
    }
    // South East
    if (!sameTerrain(x + 1,  y + 1, w, h, currentTerrain, pixels)) {
      mask = 19;
    }
  }

  if (mask != 15) {
    if (otherTerrain >= 0) {
      tilemaptile->bottom = { otherTerrain, 0, 0 };
    } else {
      tilemaptile->bottom = { 0, 0, 0 };
    }
  }

  switch(mask) {
    case 0:
      tilemaptile->top = { currentTerrain,  0, 96 };
      break;
    case 1:
      tilemaptile->top = { currentTerrain,  0, 80 };
      break;
    case 2:
      tilemaptile->top = { currentTerrain, 48, 96 };
      break;
    case 3:
      tilemaptile->top = { currentTerrain, 48, 80 };
      break;
    case 4:
      tilemaptile->top = { currentTerrain, 16, 96 };
      break;
    case 5:
      tilemaptile->top = { currentTerrain, 16, 80 };
      break;
    case 6:
      tilemaptile->top = { currentTerrain, 32, 96 };
      break;
    case 7:
      tilemaptile->top = { currentTerrain, 32, 80 };
      break;
    case 8:
      tilemaptile->top = { currentTerrain,  0, 48 };
      break;
    case 9:
      tilemaptile->top = { currentTerrain,  0, 64 };
      break;
    case 10:
      tilemaptile->top = { currentTerrain, 48, 48 };
      break;
    case 11:
      tilemaptile->top = { currentTerrain, 48, 64 };
      break;
    case 12:
      tilemaptile->top = { currentTerrain, 16, 48 };
      break;
    case 13:
      tilemaptile->top = { currentTerrain, 16, 64 };
      break;
    case 14:
      tilemaptile->top = { currentTerrain, 32, 48 };
      break;
    case 15:
      tilemaptile->bottom = {
        currentTerrain,
        (currentTile*16) % t->getWidth(),
        (currentTile*16) / t->getWidth()
      };
      break;
    case 16:
      tilemaptile->top = { currentTerrain, 80, 80 };
      break;
    case 17:
      tilemaptile->top = { currentTerrain, 64, 80 };
      break;
    case 18:
      tilemaptile->top = { currentTerrain, 80, 64 };
      break;
    case 19:
      tilemaptile->top = { currentTerrain, 64, 64 };
      break;
    default:
      std::cout << "missing: " << mask << std::endl;
  }
}


class AutoTileSystem : public SetupSystem, public RenderSystem {
  private:
    SDL_Renderer* renderer;
    SDL_Window* window;

    constexpr static int dstTileSize = 32;
    constexpr static int srcTileSize = 16;
    
    const std::string mapfile = "assets/tilemaps/3.png";
    const std::string layerfiles[3] = {
      "assets/tilesets/Water.png",
      "assets/tilesets/Dirt.png",
      "assets/tilesets/Grass.png",
    };

    SDL_Texture* tilesets[3];

    Tile* tilemap;
    int tilemapWidth;
    int tilemapHeight;
    
  public:
    AutoTileSystem(SDL_Renderer* r, SDL_Window* w) : renderer(r), window(w) {}

    ~AutoTileSystem() {}


    // setup
    void run() override {
      for(int i = 0; i < 3; i++) {
        SDL_Surface* surface = IMG_Load(layerfiles[i].c_str());
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        tilesets[i] = texture;
        SDL_FreeSurface(surface);
      }

      STexture* t = new STexture(renderer, window);
      t->load(mapfile);
      tilemapWidth = t->getWidth();
      tilemapHeight = t->getHeight();      
      tilemap = new Tile[tilemapWidth * tilemapHeight];

      t->lockTexture();
      for(int y = 0; y < tilemapHeight; y++) {
        for(int x = 0; x < tilemapWidth; x++) {
          Uint32 currentColor = t->getPixel(x, y);
          int r = ((int)(currentColor >> 16) & 0xff);
          int g = ((int)(currentColor >> 8) & 0xff);
          
          autoTiling2(t, x, y, &tilemap[y*tilemapWidth + x]);
        }
      }
      t->unlockTexture();
      delete t;
    }

    // autoTiling(t, x, y, &tilemap[i]);

    void run(SDL_Renderer* r) override {
      glm::mat4 viewProj = RenderSystem::scene->getMainCameraViewProj();
      // Entity* c = RenderSystem::scene->getMainCamera();
      
      SDL_Rect dst = { 0, 0, dstTileSize, dstTileSize };

      for(int y = 0; y < tilemapHeight; y++) {
        for(int x = 0; x < tilemapWidth; x++) {
          Tile tile = tilemap[y*tilemapWidth + x];
          SDL_Rect src = { tile.bottom.x, tile.bottom.y, srcTileSize, srcTileSize };
          SDL_RenderCopy(r,
            tilesets[tile.bottom.index],
            &src,
            &dst
          );
          if (tile.top.index != -1) {
            SDL_Rect src = { tile.top.x, tile.top.y, srcTileSize, srcTileSize };
            SDL_RenderCopy(r,
              tilesets[tile.top.index],
              &src,
              &dst
            );
          }
          dst.x += dstTileSize;
        }
        dst.x = 0;
        dst.y += dstTileSize;
      }
    }
};

#endif