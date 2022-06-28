#include <iostream>	
#include <GLFW/glfw3.h>
// #include "Math/math.h"

// #include "math.cpp"

// we need a header if we dont include
// float add(float a, float b);

GLFWwindow *window;

int main(int argc, const char * argv[]) {
    std::cout << "Hello World!" << std::endl;

    if (!glfwInit())
    {
      fprintf(stderr, "Failed to initialize\n");
      exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(300, 300, "Sample", NULL, NULL);

    if (!window)
    {
      fprintf(stderr, "Failed to open window\n");
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    while(!glfwWindowShouldClose(window))
    {
      glfwSwapBuffers(window);
      glfwPollEvents();
    }



    return 0;
} 

