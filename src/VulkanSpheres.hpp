#pragma once

#include "ShaderData.hpp"

class VulkanSpheres
{

};

int generateSphereVertices(float radius, int sectors, int stacks,
                           Vertex vertices[]);
int generateSphereIndices(int sectors, int stacks, uint32_t indices[]);