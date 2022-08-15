#include <iostream>
#include <string>
#include "Game/Game.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

Game *game = NULL;

/*
void Print(int a)
{
  std::cout << a << std::endl;
}

void Print(std::string a)
{
  std::cout << a << std::endl;
}

template<typename T>
void PrintWithTemplate(T a)
{
  std::cout << a << std::endl;
}

template<int N>
class MyArray {
  private:
    int array[N];
  
  public:
    int len() { return N; }
};

// metaprogramacion
// estamos programando no lo que hace
// nuestro programa, si no lo que hace el compilador
// el compilador escribe codigo por nosotros
*/

int main(int argc, char* args[] )
{
  /*
  Print(5);
  Print("Hello");
  PrintWithTemplate<float>(5);
  PrintWithTemplate<std::string>("Hello");
  PrintWithTemplate<float>(5.1);
  MyArray<10> a;
  Print(a.len());
  */

    game = new Game();

    game->init("Brand new game", SCREEN_WIDTH, SCREEN_HEIGHT);
    game->setup();

    while (true && game->running())
    {
      game->frameStart();
      game->handleEvents();
      game->update();
      game->render();
      game->frameEnd();
    }

    game->clean();
  return 0;
}