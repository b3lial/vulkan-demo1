#include "Sphere.hpp"

Sphere::Sphere(const Eigen::Vector3d &initialPosition,
               const Eigen::Vector3d &initialVelocity, double diameter)
    : pos(initialPosition), lastPos(initialPosition - initialVelocity * 0.1),
      vel(initialVelocity), radius(diameter / 2.0)
{
}

std::optional<CollisionInfo> Sphere::computeExitDir(const Sphere &other)
{
    const Eigen::Vector3d toOther(other.pos - pos);

    const double dist = toOther.norm();

    const double minDist = (other.radius + this->radius);

    if (dist >= minDist)
    {
        return std::nullopt;
    }

    if (dist == 0)
    {
        // FIXME random vector
        return CollisionInfo{.depth = radius,
                             .exitDir = -Eigen::Vector3d::UnitZ()};
    }

    return CollisionInfo{.depth = minDist - dist, .exitDir = -toOther / dist};
}
