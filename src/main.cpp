#include "SphereTools.hpp"
#include "vulkandemoapplication.hpp"

int main()
{
    // verify whether aligning works correctly
    static_assert(sizeof(Light) == 32, "Light must be exactly 32 bytes");

    // our renderer and window creation
    VulkanDemoApplication app;

    // add sphere vertex
    auto vertices = generateSphereVertices(0.5, 48, 32);
    app.setVertices(vertices);
    auto indices = generateSphereIndices(48, 32);
    app.setIndices(indices);

    // add lights
    std::vector<Light> lights;
    Light l1 = {glm::vec3(0.0f, 1.0, -1.0), glm::vec3(1, 0, 0)};
    Light l2 = {glm::vec3(-1.0f, 0.0f, 0.5), glm::vec3(0, 1, 0)};
    Light l3 = {glm::vec3(-1.0f, -1.0f, -0.5), glm::vec3(0, 0, 1)};
    lights.push_back(l1);
    lights.push_back(l2);
    lights.push_back(l3);
    app.setLights(lights);

    // start the application
    app.run();

    return EXIT_SUCCESS;
}
