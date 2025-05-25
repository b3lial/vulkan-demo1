#pragma once

#include "Sphere.hpp"
#include <vector>

class WorldCube
{
  public:
    static constexpr size_t numSpheres = 40;
    static constexpr double sphereSize = 0.2;
    static constexpr double initialDist = 0.02;

    WorldCube();
    void stepWorld();
    const std::vector<Sphere> &getSpheres() const { return spheres; }

    using Side = Eigen::Hyperplane<double, 3>;
    const std::array<Side, 6> &getSides() const { return sides; }
    const double getEdgeLength() const { return sideSize; }

  private:
    double sideSize;
    const std::array<Side, 6> sides;

    void updateObjects();
    void checkSphereSphereCollisions();
    void checkSphereCubeCollisions();

    const double dt = 0.0001;

    std::vector<Sphere> spheres;
};
