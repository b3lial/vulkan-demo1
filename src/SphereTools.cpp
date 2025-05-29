#include <math.h>

#include "SphereTools.hpp"

int generateSphereVertices(float radius, int sectors, int stacks,
                           Vertex vertices[])
{
    const float PI = 3.14159265359f;
    int index = 0;

    for (int i = 0; i <= stacks; ++i)
    {
        float stackAngle = PI / 2 - i * PI / stacks; // von +π/2 bis -π/2
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j)
        {
            float sectorAngle = j * 2 * PI / sectors;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            glm::vec3 position = glm::vec3(x, y, z);
            glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

            vertices[index] = {position, color, normal};
            index++;
        }
    }

    return index;
}

int generateSphereIndices(int sectors, int stacks, uint32_t indices[])
{
    int index = 0;

    for (int i = 0; i < stacks; ++i)
    {
        int stackStart = i * (sectors + 1);
        int nextStackStart = (i + 1) * (sectors + 1);

        for (int j = 0; j < sectors; ++j)
        {
            uint32_t topLeft = stackStart + j;
            uint32_t bottomLeft = nextStackStart + j;
            uint32_t topRight = stackStart + j + 1;
            uint32_t bottomRight = nextStackStart + j + 1;

            // Dreieck 1
            indices[index] = topLeft;
            index++;
            indices[index] = bottomLeft;
            index++;
            indices[index] = topRight;
            index++;

            // Dreieck 2
            indices[index] = topRight;
            index++;
            indices[index] = bottomLeft;
            index++;
            indices[index] = bottomRight;
            index++;
        }
    }

    return index;
}