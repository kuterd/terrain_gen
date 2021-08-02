cmake_minimum_required(VERSION 3.2.0)

project(terrain)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(terrain main.c perlin.c math.c)
target_link_libraries(terrain glfw GLEW::GLEW OpenGL m)
