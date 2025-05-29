#include "SphereTools.hpp"
#include "WorldCube.hpp"
#include "vulkandemoapplication.hpp"

int main()
{
    // verify whether aligning works correctly
    static_assert(sizeof(Light) == 32, "Light must be exactly 32 bytes");

    // manages spheres, moves them, checks for collisions
    WorldCube worldCube;

    // our renderer and window creation
    VulkanDemoApplication app(worldCube);

    // add sphere vertex
    Vertex vertices[SPHERE_VERTICES];
    int verticesSize = generateSphereVertices(0.5, SPHERE_VERTICE_SECTORS, SPHERE_VERTICE_STACKS, vertices);
    app.setVertices(vertices, verticesSize);
    uint32_t indices[SPHERE_INDICES];
    int indicesSize = generateSphereIndices(SPHERE_VERTICE_SECTORS, SPHERE_VERTICE_STACKS, indices);
    app.setIndices(indices, indicesSize);

    // add lights
    Light lights[LIGHTS_AMOUNT];
    lights[0] = {glm::vec3(0.0f, 1.0, -1.0), glm::vec3(1, 0, 0)};
    lights[1] = {glm::vec3(-1.0f, 0.0f, 0.5), glm::vec3(0, 1, 0)};
    lights[2] = {glm::vec3(-1.0f, -1.0f, -0.5), glm::vec3(0, 0, 1)};
    app.setLights(lights, LIGHTS_AMOUNT);

    // start the application
    app.run();

    return EXIT_SUCCESS;
}
