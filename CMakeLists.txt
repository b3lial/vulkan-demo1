cmake_minimum_required(VERSION 3.10)
PROJECT(vulkan_demo1)
SET(PROJECT_VERSION 1.0)
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
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(STATUS "Configuring for Clang on macOS (optimized for size)")
    set(CMAKE_CXX_FLAGS_RELEASE "-Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,-dead_strip")

elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "Configuring for GCC (optimized for size)")
    set(CMAKE_CXX_FLAGS_RELEASE "-Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s -Wl,--gc-sections")

else()
    message(WARNING "Unknown compiler/platform – no size optimization flags set.")
endif()

ADD_EXECUTABLE(vulkan_demo1
    src/Main.cpp
    src/Sphere.cpp
    src/WorldCube.cpp
    src/VulkanDemoApplication.cpp
    src/VulkanSpheres.cpp
    src/VulkanGrid.cpp
    src/ShaderData.cpp
    src/EmbeddedShaders.cpp
)
target_link_libraries (vulkan_demo1
    PUBLIC
        Eigen3::Eigen
        glfw
        Vulkan::Vulkan
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
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${SPIRV_DIR})
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Find xxd command
find_program(XXD_COMMAND xxd)
if(NOT XXD_COMMAND)
    message(FATAL_ERROR "xxd command not found. Please install xxd.")
endif()

set(SHADERS
    shader.vert
    shader.frag
    grid.vert
    grid.frag
)

foreach(SHADER ${SHADERS})
    set(SRC ${SHADER_DIR}/${SHADER})
    set(SPIRV ${SPIRV_DIR}/${SHADER}.spv)
    set(HEADER ${GENERATED_DIR}/${SHADER}_spv.h)

    # Compile shader to SPIR-V
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND glslang::validator
        ARGS --target-env vulkan1.0 ${SRC} --quiet -o ${SPIRV}
        DEPENDS ${SRC}
        COMMENT "Compiling ${SHADER} to SPIR-V"
        VERBATIM
    )

    # Convert SPIR-V to header file with xxd
    string(REPLACE "." "_" VARIABLE_NAME ${SHADER})
    add_custom_command(
        OUTPUT ${HEADER}
        COMMAND bash -c "${XXD_COMMAND} -i ${SHADER}.spv | sed -e 's/unsigned char/const unsigned char/g' -e 's/unsigned int/const unsigned int/g' > ${HEADER}"
        WORKING_DIRECTORY ${SPIRV_DIR}
        DEPENDS ${SPIRV}
        COMMENT "Converting ${SHADER}.spv to header"
        VERBATIM
    )

    list(APPEND SPIRV_OUTPUTS ${SPIRV})
    list(APPEND HEADER_OUTPUTS ${HEADER})
endforeach()

add_custom_target(compile_shaders ALL DEPENDS ${SPIRV_OUTPUTS} ${HEADER_OUTPUTS})
add_dependencies(vulkan_demo1 compile_shaders)

# Include the generated headers directory
target_include_directories(vulkan_demo1 PRIVATE ${GENERATED_DIR})
