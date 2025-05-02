#include "Sphere.hpp"

Sphere::Sphere(const Eigen::Vector3d &initialPosition,
               const Eigen::Vector3d &initialVelocity, double size)
    : pos(initialPosition), vel(initialVelocity), size(size)
{
}

std::optional<CollisionInfo> Sphere::computeExitDir(const Sphere &other)
{
    const Eigen::Vector3d toOther(other.pos - pos);

    const double dist = toOther.norm();

    const double minDist = other.size + this->size;

    if (dist >= minDist)
    {
        return std::nullopt;
    }

    return CollisionInfo{.depth = minDist - dist, .exitDir = -toOther / dist};
}
