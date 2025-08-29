#pragma once

#include <cstddef>

// Forward declarations for embedded shader data
// These will be defined in the generated header files

// Shader vertex
extern const unsigned char shader_vert_spv[];
extern const unsigned int shader_vert_spv_len;

// Shader fragment  
extern const unsigned char shader_frag_spv[];
extern const unsigned int shader_frag_spv_len;

// Grid vertex
extern const unsigned char grid_vert_spv[];
extern const unsigned int grid_vert_spv_len;

// Grid fragment
extern const unsigned char grid_frag_spv[];
extern const unsigned int grid_frag_spv_len;

// Cube fragment
extern const unsigned char cube_frag_spv[];
extern const unsigned int cube_frag_spv_len;

// Convenience struct for shader data
struct ShaderData {
    const unsigned char* data;
    size_t size;
};

// Accessor functions for getting shader data
inline ShaderData getShaderVertData() {
    return {shader_vert_spv, shader_vert_spv_len};
}

inline ShaderData getShaderFragData() {
    return {shader_frag_spv, shader_frag_spv_len};
}

inline ShaderData getGridVertData() {
    return {grid_vert_spv, grid_vert_spv_len};
}

inline ShaderData getGridFragData() {
    return {grid_frag_spv, grid_frag_spv_len};
}

inline ShaderData getCubeFragData() {
    return {cube_frag_spv, cube_frag_spv_len};
}