#include "VulkanDemoApplication.hpp"
#include "WorldCube.hpp"

int main()
{
    // manages spheres, moves them, checks for collisions
    WorldCube worldCube;

    // our renderer and window creation
    VulkanDemoApplication app(worldCube);

    // start the application
    app.run();

    return EXIT_SUCCESS;
}
