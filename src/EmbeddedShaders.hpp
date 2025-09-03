#pragma once

#include <cstddef>

// Forward declarations for embedded shader data
// These will be defined in the generated header files

// Object vertex
extern const unsigned char object_vert_spv[];
extern const unsigned int object_vert_spv_len;

// Object fragment  
extern const unsigned char object_frag_spv[];
extern const unsigned int object_frag_spv_len;

// Grid vertex
extern const unsigned char grid_vert_spv[];
extern const unsigned int grid_vert_spv_len;

// Grid fragment
extern const unsigned char grid_frag_spv[];
extern const unsigned int grid_frag_spv_len;


// Convenience struct for shader data
struct ShaderData {
    const unsigned char* data;
    size_t size;
};

// Accessor functions for getting shader data
inline ShaderData getObjectVertData() {
    return {object_vert_spv, object_vert_spv_len};
}

inline ShaderData getObjectFragData() {
    return {object_frag_spv, object_frag_spv_len};
}

inline ShaderData getGridVertData() {
    return {grid_vert_spv, grid_vert_spv_len};
}

inline ShaderData getGridFragData() {
    return {grid_frag_spv, grid_frag_spv_len};
}

