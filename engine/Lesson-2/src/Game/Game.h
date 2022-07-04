#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Game {
  public:
    Game();
    ~Game();

    void init(const char* title, int width, int height);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running();

  private:
    int counter;
    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;
};