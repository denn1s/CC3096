cmake_minimum_required(VERSION 3.23)

project(SampleP VERSION 1.0)

add_executable(SampleE src/main.cpp)

target_link_libraries(
  SampleE
  lua
)