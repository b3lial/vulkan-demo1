#include "SphereTools.hpp"
#include <cmath>

std::vector<Vertex> generateSphereVertices(float radius, int sectors, int stacks) {
    std::vector<Vertex> vertices;

    const float PI = 3.14159265359f;

    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = PI / 2 - i * PI / stacks; // von +π/2 bis -π/2
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * 2 * PI / sectors;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            vertices.push_back({{ glm::vec3(x, y, z) }, {1.0f, 0.0f, 0.0f}});
        }
    }

    return vertices;
}