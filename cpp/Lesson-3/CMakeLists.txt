cmake_minimum_required(VERSION 3.23)

project(SampleP VERSION 1.0)

add_executable(SampleE src/main.cpp)

add_subdirectory(src/Math)

target_link_directories(SampleE PRIVATE src/Math)

target_link_libraries(SampleE math)