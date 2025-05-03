#include "SphereTools.hpp"
#include "vulkandemoapplication.hpp"

#include <iostream>

int main()
{
    // verify whether aligning works correctly
    static_assert(sizeof(Light) == 32, "Light must be exactly 32 bytes");

    // our renderer and window creation
    VulkanDemoApplication app;

    // add spheres
    auto vertices = generateSphereVertices(0.5, 48, 32);
    app.setVertices(vertices);
    auto indices = generateSphereIndices(48, 32);
    app.setIndices(indices);
    std::vector<AnimatedBody> spheres = {{{0.0f, 0.0f, 0.0f}, 0.5f, 0.4f, 0},
                                         {{-1.0f, 0.0f, -0.4f}, 0.5f, 0.5f, 0},
                                         {{1.0f, 0.0f, -0.3f}, 0.3f, 0.4f, 0}};
    app.setSpheres(spheres);

    // add lights
    std::vector<Light> lights;
    Light l1 = {glm::vec3(0.0f, 1.0, -1.0), glm::vec3(1, 0, 0)};
    Light l2 = {glm::vec3(-1.0f, 0.0f, 0.5), glm::vec3(0, 1, 0)};
    lights.push_back(l1);
    lights.push_back(l2);
    app.setLights(lights);

    // set position of eye
    app.setView(glm::vec3(4.0f, 4.0f, 4.0f));

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
