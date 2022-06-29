cmake_minimum_required(VERSION 3.23)

project(SDLDEMO VERSION 1.0)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME}
  SDL2main
  SDL2
)