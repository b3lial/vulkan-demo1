#include "VulkanDemoApplication.hpp"
#include "WorldCube.hpp"
#include "Logger.hpp"

int main()
{
    // manages spheres, moves them, checks for collisions
    WorldCube worldCube;

#ifdef DEBUG
    // Debug: Print corners for each side of the cube
    const WorldCube::Side* sides = worldCube.getSides();
    double edgeLength = worldCube.getEdgeLength();
    LOG_DEBUG("Side length: " + std::to_string(edgeLength));
    
    for (int i = 0; i < 6; ++i) {
        glm::dvec3 corners[4];
        sides[i].getCorners(corners, edgeLength);
        
        LOG_DEBUG("Side " + std::to_string(i) + " corners:");
        for (int j = 0; j < 4; ++j) {
            LOG_DEBUG("  Corner " + std::to_string(j) + ": (" + 
                      std::to_string(corners[j].x) + ", " + 
                      std::to_string(corners[j].y) + ", " + 
                      std::to_string(corners[j].z) + ")");
        }
        LOG_DEBUG("");
    }
#endif

    // our renderer and window creation
    VulkanDemoApplication app(worldCube);

    // start the application
    app.run();

    return EXIT_SUCCESS;
}
