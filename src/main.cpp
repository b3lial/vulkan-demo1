#include "vulkandemoapplication.hpp"
#include "SphereTools.hpp"

#include <iostream>

int main()
{
    static_assert(sizeof(Light) == 32, "Light must be exactly 32 bytes");
    
    VulkanDemoApplication app;

    // add a sphere
    auto vertices = generateSphereVertices(0.5, 48, 32);
    app.setVertices(vertices);
    auto indices = generateSphereIndices(48, 32);
    app.setIndices(indices);

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
