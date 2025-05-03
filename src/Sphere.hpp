#pragma once
#include <Eigen/Geometry>
#include <optional>

struct CollisionInfo
{
    const double depth = 0;
    // vector moving the objects out of each other
    const Eigen::Vector3d exitDir;
};

class Sphere
{
  public:
    Sphere(const Eigen::Vector3d &initialPosition,
           const Eigen::Vector3d &initialVelocity, double size);

    /**
     * Checks for intersection.
     * If an intersection exists, the shortest vector
     * solving the intersection is returned
     */
    std::optional<CollisionInfo> computeExitDir(const Sphere &other);

    void setPos(const Eigen::Vector3d &newPos) { pos = newPos; }
    const Eigen::Vector3d &getPos() const { return pos; }
    void setLastPos() { lastPos = pos; }
    void setLastPos(const Eigen::Vector3d &newPos) { lastPos = newPos; }
    const Eigen::Vector3d &getLastPos() const { return lastPos; }

    const Eigen::Vector3d &getVel() const { return vel; }
    void setVel(const Eigen::Vector3d &newVel) { vel = newVel; }

    double getSize() const { return size; }

  private:
    Eigen::Vector3d pos;
    Eigen::Vector3d lastPos;
    Eigen::Vector3d vel;

    double size;
};
