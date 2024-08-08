cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

include_directories(/usr/include/GL)
include_directories(/usr/include/GLFW)
include_directories(/usr/include/glm)

link_directories(/usr/lib/x86_64-linux-gnu)

add_executable(OpenGL_Project main.cpp)

target_link_libraries(OpenGL_Project GL GLEW glfw)
