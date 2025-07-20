#pragma once

#include "WorldSphere.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class WorldCube
{
  public:
    static constexpr unsigned int numSpheres = 40;
    static constexpr double sphereSize = 0.2;
    static constexpr double initialDist = 0.02;

    WorldCube();
    void stepWorld();
    const WorldSphere* getSpheres() const { return spheres; }
    int getSpheresSize() const { return spheresSize; }

    struct Side {
        glm::dvec3 point;
        glm::dvec3 normal_vec;
        
        Side(const glm::dvec3& p, const glm::dvec3& n) : point(p), normal_vec(normalize(n)) {}
        
        double signedDistance(const glm::dvec3& pos) const {
            return dot(pos - point, normal_vec);
        }
        
        glm::dvec3 projection(const glm::dvec3& pos) const {
            return pos - signedDistance(pos) * normal_vec;
        }
        
        glm::dvec3 normal() const { return normal_vec; }
    };
    const Side *getSides() const { return sides; }
    const double getEdgeLength() const { return sideSize; }

  private:
    double sideSize;
    const Side sides[6];

    void updateObjects();
    void checkSphereSphereCollisions();
    void checkSphereCubeCollisions();

    const double dt = 0.0001;

    WorldSphere spheres[numSpheres+1];
    unsigned int spheresSize = 0;
};
