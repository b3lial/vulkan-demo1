#pragma once

#include "Sphere.hpp"

class WorldCube
{
  public:
    static constexpr unsigned int numSpheres = 40;
    static constexpr double sphereSize = 0.2;
    static constexpr double initialDist = 0.02;

    WorldCube();
    void stepWorld();
    const Sphere* getSpheres() const { return spheres; }
    int getSpheresSize() const { return spheresSize; }

    using Side = Eigen::Hyperplane<double, 3>;
    const Side *getSides() const { return sides; }
    const double getEdgeLength() const { return sideSize; }

  private:
    double sideSize;
    const Side sides[6];

    void updateObjects();
    void checkSphereSphereCollisions();
    void checkSphereCubeCollisions();

    const double dt = 0.0001;

    Sphere spheres[numSpheres+1];
    unsigned int spheresSize = 0;
};
