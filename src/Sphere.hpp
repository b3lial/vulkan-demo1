#pragma once
#include <Eigen/Geometry>

struct CollisionInfo
{
    const double depth = 0;
    // vector moving the objects out of each other
    const Eigen::Vector3d exitDir;
    // workaround to replace optional<>
    bool isValid;
};

class Sphere
{
  public:
    Sphere() = default;
    Sphere(const Eigen::Vector3d &initialPosition,
           const Eigen::Vector3d &initialVelocity, double diameter);

    /**
     * Checks for intersection.
     * If an intersection exists, the shortest vector
     * solving the intersection is returned
     */
    CollisionInfo computeExitDir(const Sphere &other);

    void setPos(const Eigen::Vector3d &newPos) { pos = newPos; }
    const Eigen::Vector3d &getPos() const { return pos; }
    void setLastPos() { lastPos = pos; }
    void setLastPos(const Eigen::Vector3d &newPos) { lastPos = newPos; }
    const Eigen::Vector3d &getLastPos() const { return lastPos; }

    const Eigen::Vector3d &getVel() const { return vel; }
    void setVel(const Eigen::Vector3d &newVel) { vel = newVel; }

    double getDiameter() const { return radius * 2.0; }
    double getRadius() const { return radius; }

  private:
    Eigen::Vector3d pos;
    Eigen::Vector3d lastPos;
    Eigen::Vector3d vel;

    double radius;
};
