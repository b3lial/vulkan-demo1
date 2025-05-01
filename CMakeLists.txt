

PROJECT(vulkan_demo1)
SET(PROJECT_VERSION 1.0)
cmake_minimum_required(VERSION 3.22)
INCLUDE(FindPkgConfig)

find_package(glfw3 REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIRS})
link_directories(${GLFW3_INCLUDE_DIRS})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ggdb")

ADD_EXECUTABLE(vulkan_demo1
    src/main.cpp)
target_link_libraries (vulkan_demo1
    PUBLIC
        Eigen3::Eigen
        glfw
        vulkan
)

