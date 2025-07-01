PROJECT(vulkan_demo1)
SET(PROJECT_VERSION 1.0)
cmake_minimum_required(VERSION 3.22)
INCLUDE(FindPkgConfig)

find_package(glfw3 REQUIRED)
find_package (Vulkan REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(glm REQUIRED)
include_directories(${GLFW3_INCLUDE_DIRS})
link_directories(${GLFW3_INCLUDE_DIRS})

add_library(cpp20 INTERFACE)
target_compile_features(cpp20 INTERFACE cxx_std_20)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipoSupported)

function(add_lto)
    set_property(TARGET ${ARGV} PROPERTY INTERPROCEDURAL_OPTIMIZATION ${ipoSupported})
    if(${ipoSupported})
        foreach(target ${ARGV})
            target_compile_options(${target} PRIVATE -flto=auto)
        endforeach()
    endif()
endfunction()

add_executable (glslang::validator IMPORTED)
find_program (GLSLANG_VALIDATOR "glslangValidator" HINTS $ENV{VULKAN_SDK}/bin REQUIRED)
set_property (TARGET glslang::validator PROPERTY IMPORTED_LOCATION "${GLSLANG_VALIDATOR}")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

ADD_EXECUTABLE(vulkan_demo1
    src/Main.cpp
    src/Sphere.cpp
    src/WorldCube.cpp
    src/VulkanDemoApplication.cpp
    src/VulkanSpheres.cpp
    src/VulkanGrid.cpp
    src/ShaderData.cpp
)
target_link_libraries (vulkan_demo1
    PUBLIC
        Eigen3::Eigen
        glfw
        vulkan
        cpp20
        glm::glm
)

add_lto(vulkan_demo1)
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
    grid.vert
    grid.frag
)



foreach(SHADER ${SHADERS})
    set(SRC ${SHADER_DIR}/${SHADER})
    set(SPIRV ${SPIRV_DIR}/${SHADER}.spv)

    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND glslang::validator
        ARGS --target-env vulkan1.0 ${SRC} --quiet -o ${SPIRV}
        #WORKING_DIRECTORY ${SHADERS_DIR}
        #COMMAND glslc ${SRC} -o ${SPIRV}
        DEPENDS ${SRC}
        COMMENT "Compiling ${SHADER} to SPIR-V"
        VERBATIM
    )

    list(APPEND SPIRV_OUTPUTS ${SPIRV})
endforeach()

add_custom_target(compile_shaders ALL DEPENDS ${SPIRV_OUTPUTS})
add_dependencies(vulkan_demo1 compile_shaders)
