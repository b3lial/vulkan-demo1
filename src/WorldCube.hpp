#pragma once

#include "Sphere.hpp"
#include <vector>

class WorldCube
{
  public:
    WorldCube();

    void stepWorld();

    const std::vector<Sphere> &getSpheres() const
    {
      return spheres;
    }

  private:

    void updateObjects();
    void checkSphereSphereCollisions();
    void checkSphereCubeCollisions();

    const double dt = 0.0001;
    double sideSize;


    std::vector<Sphere> spheres;
};
