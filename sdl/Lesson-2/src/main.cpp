#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main( int argc, char* args[] )
{
    //The window we'll be rendering to
    SDL_Window* window = NULL;
    
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
    }
    
    
    //Create window
    window = SDL_CreateWindow(
      "SDL Tutorial",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glClearColor(1.f, 0.f, 1.f, 0.f);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    bool isRunning = true;
    SDL_Event event;

    while (isRunning)
    {
      while (SDL_PollEvent(&event) != 0)
      {
        if (event.type == SDL_QUIT)
        {
          isRunning = false;
        }  
      }

      glClear(GL_COLOR_BUFFER_BIT);

      SDL_GL_SwapWindow(window);
    }
    

        //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}