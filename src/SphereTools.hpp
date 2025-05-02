#include <vector>
#include "vulkandemoapplication.hpp"

std::vector<Vertex> generateSphereVertices(float radius, int sectors, int stacks);
std::vector<uint32_t> generateSphereIndices(int sectors, int stacks);