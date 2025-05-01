

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

target_link_libraries(vulkan_demo1
    PUBLIC
        Eigen3::Eigen
        glfw
        vulkan
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

# === Shader Compilation ===
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SPIRV_DIR ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SPIRV_DIR})

set(SHADERS
    shader.vert
    shader.frag
)

foreach(SHADER ${SHADERS})
    set(SRC ${SHADER_DIR}/${SHADER})
    set(SPIRV ${SPIRV_DIR}/${SHADER}.spv)

    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND glslc ${SRC} -o ${SPIRV}
        DEPENDS ${SRC}
        COMMENT "Compiling ${SHADER} to SPIR-V"
        VERBATIM
    )

    list(APPEND SPIRV_OUTPUTS ${SPIRV})
endforeach()

add_custom_target(compile_shaders ALL DEPENDS ${SPIRV_OUTPUTS})
add_dependencies(vulkan_demo1 compile_shaders)
