#include "WorldSphere.hpp"

WorldSphere::WorldSphere(const glm::dvec3 &initialPosition,
               const glm::dvec3 &initialVelocity, double diameter)
    : pos(initialPosition), lastPos(initialPosition - initialVelocity * 0.1),
      vel(initialVelocity), radius(diameter / 2.0)
{
}

CollisionInfo WorldSphere::computeExitDir(const WorldSphere &other)
{
    const glm::dvec3 toOther(other.pos - pos);

    const double dist = length(toOther);

    const double minDist = (other.radius + this->radius);

    if (dist >= minDist)
    {
        return CollisionInfo{.isValid=false};
    }

    if (dist == 0)
    {
        // FIXME random vector
        return CollisionInfo{.depth = radius,
                             .exitDir = -glm::dvec3(0, 0, 1),
                             .isValid=true};
    }

    return CollisionInfo{.depth = minDist - dist, .exitDir = -toOther / dist, .isValid=true};
}
