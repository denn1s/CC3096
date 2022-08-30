#include <vector>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "./STexture.cpp"
#include "./System.h"
#include "./Components.hpp"


class MovementSystem : public UpdateSystem {
  private:
    int counter = 0;

  public:
    MovementSystem(int c) : counter(c) {}

    void run(double dT) override {
      const auto view = scene->r.view<TransformComponent, MovementComponent>();
      for (const entt::entity e : view) {
        TransformComponent& t = view.get<TransformComponent>(e);
        MovementComponent& m = view.get<MovementComponent>(e);

        if (t.position.x <= 0)
        {
          m.velocity.x *= -1;
        }

        if (t.position.x >= 640)
        {
          m.velocity.x *= -1;
        }

        if (t.position.y <= 0)
        {
          m.velocity.y *= -1;
        }

        if (t.position.y >= 480)
        {
          m.velocity.y *= -1;
        }

        t.position.x += m.velocity.x * dT;
        t.position.y += m.velocity.y * dT;

        std::cout << "x: " << t.position.x << std::endl;
        std::cout << "y: " << t.position.y << std::endl;
        std::cout << "c: " << counter++ << std::endl;
      }
    }
};

class CubeSystem : public RenderSystem {
  public:
    void run(SDL_Renderer* renderer) override {
      SDL_SetRenderDrawColor(renderer, 255, 100, 100, 1);

      const auto view = scene->r.view<TransformComponent, ColliderComponent>();
      for (const entt::entity e : view) {
        const TransformComponent& t = view.get<TransformComponent>(e);
        const ColliderComponent& c = view.get<ColliderComponent>(e);
        const int x = t.position.x;
        const int y = t.position.y;
        const int w = c.size.x;
        const int h = c.size.y;

        SDL_Rect rect = { x, y, w, h };    
        SDL_RenderFillRect(renderer, &rect);
      }
    }
};

class HelloSystem : public SetupSystem {
  public:
    HelloSystem() {
      std::cout << "Hello System Constructor" << std::endl;
    }

    ~HelloSystem() {
      std::cout << "Hello System Destructor" << std::endl;
    }

    void run() override {
      std::cout << "Hello System!" << std::endl;
    }
};


class TilemapSystem : public SetupSystem, public RenderSystem {
  private:
    SDL_Renderer* renderer;

    constexpr static int tilesCount = 2;
    constexpr static int tilesWidth = 4;
    constexpr static int tilesHeight = 3;

    constexpr static int x = 0;
    constexpr static int y = 0;
    constexpr static int tileWidth = 24;
    constexpr static int tileHeigth = 24;

    constexpr static char mmap[tilesWidth][tilesHeight] = {
      {0, 0, 0},
      {0, 1, 0},
      {0, 0, 0},
      {0, 0, 0}
    };
    const std::string files[tilesCount] = {
      "assets/tiles/grass.png",
      "assets/tiles/water.png"
    };

    std::map<char, SDL_Texture*> tiles;
    SDL_Texture** tilemap;

  public:
    TilemapSystem(SDL_Renderer* r) : renderer(r) {
      std::cout << "Tile map system started" << std::endl;
      tilemap = new SDL_Texture*[tilesWidth * tilesHeight];
    }

    ~TilemapSystem() {
      /*
      for(int i = 0; i < tilesCount; i++) {
        SDL_FreeSurface(surfaces[i]);
      }
      */
    }

    // setup
    void run() override {
      SDL_Surface* surfaces[tilesCount];

      for(int i = 0; i < tilesCount; i++) {
        surfaces[i] = IMG_Load(files[i].c_str());
      }
      
      for(int i = 0; i < tilesWidth; i++) {
        for(int j = 0; j < tilesHeight; j++) {
          char surfaceIndex = mmap[i][j];
          auto found = tiles.find(surfaceIndex);
          if (found == tiles.end()) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surfaces[surfaceIndex]);
            tiles.insert(
              std::make_pair(
                surfaceIndex,
                texture
              )
            );
            tilemap[i*tilesWidth + j] = texture;
          } else {
            tilemap[i*tilesWidth + j] = found->second;
          }
        }
      }

      for(int i = 0; i < tilesCount; i++) {
        SDL_FreeSurface(surfaces[i]);
      }
    }

    void run(SDL_Renderer* r) override {
      SDL_Rect rect = { x, y, tileWidth, tileHeigth };

      for(int i = 0; i < tilesWidth; i++) {
        for(int j = 0; j < tilesHeight; j++) {
          SDL_RenderCopy(r, tilemap[i*tilesWidth + j], nullptr, &rect);
          rect.x += tileWidth;
        }
        rect.x = 0;
        rect.y += tileHeigth;
      }
    }
};


class AdvancedTilemapSystem : public SetupSystem, public RenderSystem {
  private:
    SDL_Renderer* renderer;
    SDL_Window* window;

    constexpr static int tilesCount = 2;
    constexpr static int x = 0;
    constexpr static int y = 0;
    constexpr static int tileWidth = 32;
    constexpr static int tileHeigth = 32;

    const std::string mmap = "assets/tilemaps/1.png";
    const std::string files[tilesCount] = {
      "assets/tiles/grass.png",
      "assets/tiles/water.png"
    };
    std::map<char, SDL_Texture*> tiles;
    SDL_Texture** tilemap;
    int tilesWidth;
    int tilesHeight;

  public:
    AdvancedTilemapSystem(SDL_Renderer* r, SDL_Window* w) : renderer(r), window(w) {
      std::cout << "Tile map system started" << std::endl;
    }

    ~AdvancedTilemapSystem() {
    }

    // setup
    void run() override {
      SDL_Surface* surfaces[tilesCount];

      for(int i = 0; i < tilesCount; i++) {
        surfaces[i] = IMG_Load(files[i].c_str());
      }
  
      STexture* t = new STexture(renderer, window);
      t->load(mmap);
      tilesWidth = t->getWidth();
      tilesHeight = t->getHeight();
      int totalTiles = tilesWidth * tilesHeight;

      tilemap = new SDL_Texture*[totalTiles];

      for(int i = 0; i < totalTiles; i++) {
        Uint32 currentColor = t->getPixel(i);
        int surfaceIndex = ((int)(currentColor >> 16) & 0xff) - 1;

        auto found = tiles.find(surfaceIndex);
        if (found == tiles.end()) {
          SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surfaces[surfaceIndex]);
          tiles.insert(
            std::make_pair(
              surfaceIndex,
              texture
            )
          );
          tilemap[i] = texture;
        } else {
          tilemap[i] = found->second;
        }
      }

      delete t;

      for(int i = 0; i < tilesCount; i++) {
        SDL_FreeSurface(surfaces[i]);
      }
    }

    void run(SDL_Renderer* r) override {
      SDL_Rect rect = { x, y, tileWidth, tileHeigth };

      for(int i = 0; i < tilesHeight; i++) {
        for(int j = 0; j < tilesWidth; j++) {
          SDL_RenderCopy(r, tilemap[i*tilesWidth + j], nullptr, &rect);
          rect.x += tileWidth;
        }
        rect.x = 0;
        rect.y += tileHeigth;
      }
    }
};


class TileSetSystem : public SetupSystem, public RenderSystem {
  private:
    SDL_Renderer* renderer;
    SDL_Window* window;

    constexpr static int x = 0;
    constexpr static int y = 0;
    constexpr static int tileWidth = 32;
    constexpr static int tileHeigth = 32;

    const std::string mmap = "assets/tilemaps/2.png";
    const std::string files[3] = {
      "assets/tilesets/Dirt.png",
      "assets/tilesets/Grass.png",
      "assets/tilesets/Hills.png",
    };
    SDL_Texture* tilesets[3];
    int tilesWidth;
    int tilesHeight;

    SDL_Rect* tilemap;
    

  public:
    TileSetSystem(SDL_Renderer* r, SDL_Window* w) : renderer(r), window(w) {
      std::cout << "Tile map system started" << std::endl;
    }

    ~TileSetSystem() {
    }

    // setup
    void run() override {
      // tilesets = new SDL_Texture*[3];

      for(int i = 0; i < 3; i++) {
        SDL_Surface* surface = IMG_Load(files[i].c_str());
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        tilesets[i] = texture;
        SDL_FreeSurface(surface);
      }
  
      STexture* t = new STexture(renderer, window);
      t->load(mmap);
      tilesWidth = t->getWidth();
      tilesHeight = t->getHeight();
      const int totalTiles = tilesWidth * tilesHeight;

      tilemap = new SDL_Rect[totalTiles];

      for(int i = 0; i < totalTiles; i++) {
        Uint32 currentColor = t->getPixel(i);
        int r = ((int)(currentColor >> 16) & 0xff);
        int g = ((int)(currentColor >> 8) & 0xff);
        int b = ((int)(currentColor & 0xff));
        tilemap[i] = { r * 16, 0, 16, 16 };
      }

      delete t;
    }

    void run(SDL_Renderer* r) override {
      SDL_Rect rect = { x, y, tileWidth, tileHeigth };

      for(int i = 0; i < tilesHeight; i++) {
        for(int j = 0; j < tilesWidth; j++) {
          SDL_Rect src = tilemap[i*tilesWidth + j];
          SDL_RenderCopy(r, tilesets[0], &src, &rect);
          rect.x += tileWidth;
        }
        rect.x = 0;
        rect.y += tileHeigth;
      }
    }
};

struct Terrain {
  int index;
  int x;
  int y;
};

struct Tile {
  Terrain bottom{-1, 0, 0};
  Terrain top{-1, 0, 0};
};

class AdvancedTileSetSystem : public SetupSystem, public RenderSystem {
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
    AdvancedTileSetSystem(SDL_Renderer* r, SDL_Window* w) : renderer(r), window(w) {}

    ~AdvancedTileSetSystem() {}

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
          // std::cout << "rg: " << r << ", " << g << std::endl;
          tilemap[y*tilemapWidth + x].bottom = { r, g * srcTileSize, 0};
        }
      }
      t->unlockTexture();
      delete t;
    }

    void run(SDL_Renderer* r) override {
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
          dst.x += dstTileSize;
        }
        dst.x = 0;
        dst.y += dstTileSize;
      }
    }
};

void autoTiling(STexture* t, int x, int y, Tile* tilemaptile) {
  Uint32 currentColor = t->getPixel(x, y);
  int currentTerrain = ((int)(currentColor >> 16) & 0xff);
  int currentTile = ((int)(currentColor >> 8) & 0xff);

  int mask = 0;
  int otherTerrain = 0;

  // North
  if (y - 1 >= 0) {
    int o = ((int)(t->getPixel(x, y - 1) >> 16) & 0xff);
    if (o == currentTerrain) {
      mask += 1;
    }
  }

  // West
  if (x - 1 >= 0) {
    int o = ((int)(t->getPixel(x - 1, y) >> 16) & 0xff);
    if (o == currentTerrain) {
      mask += 2;
    }
  }

  // East
  if (x + 1 < t->getWidth()) {
    int o = ((int)(t->getPixel(x + 1, y) >> 16) & 0xff);
    if (o == currentTerrain) {
      mask += 4;
    }
  }

  // South
  if (y + 1 < t->getHeight()) {
    int o = ((int)(t->getPixel(x, y + 1) >> 16) & 0xff);
    if (o == currentTerrain) {
      mask += 8;
    }
  }

  if (mask != 15) {
    tilemaptile->bottom = { otherTerrain, 0, 0 };
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
      // tilemaptile->bottom = { currentTerrain, currentTile*16, 0 };
      // tilemaptile->top = { currentTerrain, 0, 0 };
      break;
    default:
      std::cout << "missing: " << mask << std::endl;
  }
}

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