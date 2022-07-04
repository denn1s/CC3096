#include "Game/Game.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

Game *game = NULL;

int main( int argc, char* args[] )
{
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    game = new Game();

    game->init("Brand new game", SCREEN_WIDTH, SCREEN_HEIGHT);

    while (game->running())
    {
      frameStart = SDL_GetTicks();

      game->handleEvents();
      game->update();
      game->render();

      frameTime = SDL_GetTicks() - frameStart;

      if (frameDelay > frameTime) {
        SDL_Delay(frameDelay - frameTime);
      }
    }

    game->clean();

    return 0;
}