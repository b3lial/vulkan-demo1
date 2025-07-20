#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CollisionInfo
{
    const double depth = 0;
    // vector moving the objects out of each other
    const glm::dvec3 exitDir;
    // workaround to replace optional<>
    bool isValid;
};

class WorldSphere
{
  public:
    WorldSphere() = default;
    WorldSphere(const glm::dvec3 &initialPosition,
           const glm::dvec3 &initialVelocity, double diameter);

    /**
     * Checks for intersection.
     * If an intersection exists, the shortest vector
     * solving the intersection is returned
     */
    CollisionInfo computeExitDir(const WorldSphere &other);

    void setPos(const glm::dvec3 &newPos) { pos = newPos; }
    const glm::dvec3 &getPos() const { return pos; }
    void setLastPos() { lastPos = pos; }
    void setLastPos(const glm::dvec3 &newPos) { lastPos = newPos; }
    const glm::dvec3 &getLastPos() const { return lastPos; }

    const glm::dvec3 &getVel() const { return vel; }
    void setVel(const glm::dvec3 &newVel) { vel = newVel; }

    double getDiameter() const { return radius * 2.0; }
    double getRadius() const { return radius; }

  private:
    glm::dvec3 pos;
    glm::dvec3 lastPos;
    glm::dvec3 vel;

    double radius;
};
