#include "Game/Game.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

Game *game = NULL;

int main( int argc, char* args[] )
{
    game = new Game();

    game->init("Brand new game", SCREEN_WIDTH, SCREEN_HEIGHT);

    while (game->running())
    {
      game->handleEvents();
      game->update();
      game->render();
    }

    game->clean();

    return 0;
}