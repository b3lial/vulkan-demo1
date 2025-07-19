#include "VulkanDemoApplication.hpp"
#include "WorldCube.hpp"

int main()
{
    // verify whether aligning works correctly
    static_assert(sizeof(Light) == 32, "Light must be exactly 32 bytes");

    // manages spheres, moves them, checks for collisions
    WorldCube worldCube;

    // our renderer and window creation
    VulkanDemoApplication app(worldCube);

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
