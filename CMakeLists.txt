cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)

set(CMAKE_CXX_STANDARD 17)

set(GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/glm")
include_directories(${GLM_INCLUDE_DIR})
include_directories("/usr/include/GLFW")
link_directories("/usr/lib/x86_64-linux-gnu")

find_package(OpenGL REQUIRED)

add_executable(OpenGL_Project src/main.cpp src/shader.cpp)

target_link_libraries(OpenGL_Project GL GLEW glfw)
