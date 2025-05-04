#pragma once

#include "Sphere.hpp"
#include <vector>

class WorldCube
{
  public:
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
