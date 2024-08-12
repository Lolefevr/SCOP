cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIRS})

add_executable(OpenGL_Project main.cpp obj_loader.cpp controls.cpp shaders.cpp utils.cpp)

target_link_libraries(OpenGL_Project ${OPENGL_LIBRARIES} glfw)
