#include "WorldCube.hpp"

WorldCube::WorldCube()
    : sideSize(2.0), sides{Side(glm::dvec3(1, 0, 0) * sideSize / 2.0,
                                -glm::dvec3(1, 0, 0)),
                           Side(glm::dvec3(1, 0, 0) * -sideSize / 2.0,
                                glm::dvec3(1, 0, 0)),
                           Side(glm::dvec3(0, 1, 0) * sideSize / 2.0,
                                -glm::dvec3(0, 1, 0)),
                           Side(glm::dvec3(0, 1, 0) * -sideSize / 2.0,
                                glm::dvec3(0, 1, 0)),
                           Side(glm::dvec3(0, 0, 1) * sideSize / 2.0,
                                -glm::dvec3(0, 0, 1)),
                           Side(glm::dvec3(0, 0, 1) * -sideSize / 2.0,
                                glm::dvec3(0, 0, 1))}
{
    const size_t steps = sideSize / (sphereSize + initialDist) - 1;

    auto toCoord = [&](size_t step) {
        return -sideSize / 2.0 + initialDist + (step + 1) * (sphereSize + 0.02);
    };

    unsigned int i = 0;
    for (size_t x = 0; x < steps; x++)
    {
        const double curX = toCoord(x);
        for (size_t y = 0; y < steps; y++)
        {
            const double curY = toCoord(y);
            for (size_t z = 0; z < steps; z++)
            {
                const double curZ = toCoord(z);
                spheres[i] = WorldSphere(glm::dvec3(curX, curY, curZ),
                                     glm::dvec3(0.05, 0.05, 0.1),
                                     sphereSize);
                i++;

//                 std::cout << spheres.back().getPos().transpose() << std::endl;

                if (i > numSpheres)
                {
                    break;
                }
            }
            if (i > numSpheres)
            {
                break;
            }
        }
        if (i > numSpheres)
        {
            break;
        }
    }
    spheresSize = i;
}

void WorldCube::updateObjects()
{
    for (WorldSphere &s : spheres)
    {
        const glm::dvec3 curVel = s.getPos() - s.getLastPos();
        const double velDist = length(curVel);
        //         std::cout << "Initial pos : " << s.getPos().transpose() << "
        //         last Pos : " << s.getLastPos().transpose() << " vel is : " <<
        //         curVel.transpose() << std::endl;

        // speed up !
        const double wantedVel = 0.006;
        glm::dvec3 acceleration(0.0);
        if (std::abs(velDist - wantedVel) > 1e-7)
        {
            const glm::dvec3 velNorm = normalize(curVel);

            if (velDist < 1e-10)
            {
                acceleration = -normalize(s.getPos()) * wantedVel / dt;
                // std::cout << "Velocity is zero, setting acc : " <<
                // acceleration.transpose() << std::endl;
            }
            else
            {

                acceleration = (wantedVel - velDist) / dt * velNorm;
                // std::cout << "Velocity is " << velDist << " wanted : " <<
                // wantedVel  << " , setting acc : " << acceleration.transpose()
                // << std::endl;
            }
        }

        // update for new iterration
        s.setLastPos();

        // verlet integrate
        glm::dvec3 nextPos = s.getPos() + curVel + acceleration * dt * dt;

        s.setPos(nextPos);
        // std::cout << "NExt pos : " << nextPos.transpose() << std::endl;
    }
}

void WorldCube::checkSphereSphereCollisions()
{
    // solve collisions
    for (size_t i = 0; i < spheresSize; i++)
    {
        WorldSphere &cur(spheres[i]);
        for (size_t j = i + 1; j < spheresSize; j++)
        {
            WorldSphere &other(spheres[j]);
            const auto intersection = cur.computeExitDir(other);

            if (!intersection.isValid)
            {
                continue;
            }

            //             std::cout << "Intersection : " << i << " and " << j
            //             << " depth: " << intersection->depth << " exit dir "
            //             << intersection->exitDir.transpose() << std::endl;

            const double d2 = intersection.depth / 2.0;
            cur.setPos(cur.getPos() + intersection.exitDir * d2);
            other.setPos(other.getPos() - intersection.exitDir * d2);

            //             std::cout << "New Pos  : " <<
            //             cur.getPos().transpose() << " and " <<
            //             other.getPos().transpose() << std::endl;
        }
    }
}

void WorldCube::checkSphereCubeCollisions()
{
    for (const Side &side : sides)
    {
        for (WorldSphere &s : spheres)
        {
            double distToPlane = side.signedDistance(s.getPos());
            if (distToPlane < s.getRadius())
            {
                // std::cout << "Collision with side !" << std::endl;

                glm::dvec3 vel = s.getPos() - s.getLastPos();
                double velDist = length(vel);
                glm::dvec3 velNormalized(vel / velDist);

                // std::cout << "Cur VEl is : " << velDist << std::endl;

                // Calculate intersection point manually
                double t = dot(side.point - s.getPos(), side.normal()) / dot(velNormalized, side.normal());
                glm::dvec3 travelIntersectionOnSide = s.getPos() + t * velNormalized;

                glm::dvec3 curPosOnPlane = side.projection(s.getPos());

                // std::cout << "travelIntersectionOnSide : " <<
                // travelIntersectionOnSide.transpose() << std::endl; std::cout
                // << "curPosOnPlane : " << curPosOnPlane.transpose() <<
                // std::endl;

                const double intersectionDepth = s.getRadius() - distToPlane;
                double distBackToClearIntersection;
                if (length(curPosOnPlane - travelIntersectionOnSide) < 1e-10)
                {
                    // we don't bounce off at an angle
                    distBackToClearIntersection = intersectionDepth;
                }
                else
                {
                    const glm::dvec3 velProjectedOnSide =
                        travelIntersectionOnSide - curPosOnPlane;

                    double cosAngle =
                        dot(normalize(velProjectedOnSide), velNormalized);

                    distBackToClearIntersection = intersectionDepth / cosAngle;
                }

                // Now we compute the bounce off position...
                const glm::dvec3 nonIntersectingPos(
                    s.getPos() - velNormalized * distBackToClearIntersection);

                // Reflect velocity vector across the normal
                const glm::dvec3 newVelDir = velNormalized - 2.0 * dot(velNormalized, side.normal()) * side.normal();

                double distBeforeBounce =
                    length(nonIntersectingPos - s.getPos());

                s.setPos(nonIntersectingPos +
                         newVelDir * (velDist - distBeforeBounce));

                // weired hack, preserve velocity by modifying the last pos
                s.setLastPos(nonIntersectingPos +
                             newVelDir * -distBeforeBounce);

                // std::cout << "New Pos  : " << s.getPos().transpose() << " new
                // last pos : " << s.getLastPos().transpose() << std::endl;
            }
        }
    }

    // HACK never leave the cube
    for (WorldSphere &s : spheres)
    {
        glm::dvec3 pos = s.getPos();

        for (size_t i = 0; i < 3; i++)
        {
            pos[i] = std::min(sideSize / 2.0 - s.getRadius(), pos[i]);
            pos[i] = std::max(-sideSize / 2.0 - s.getRadius(), pos[i]);
        }

        s.setPos(pos);
    }
}

void WorldCube::stepWorld()
{
    // std::cout <<std::endl << "NEW STEP !" << std::endl;
    /*
        for (WorldSphere &s : spheres)
        {
            const Eigen::Vector3d curVel = s.getPos() - s.getLastPos();
            //std::cout << "Initial pos : " << s.getPos().transpose() << " last
       Pos : " << s.getLastPos().transpose() << " vel is : " <<
       curVel.transpose() << std::endl;
        }*/

    checkSphereCubeCollisions();
    checkSphereSphereCollisions();

    updateObjects();
}
