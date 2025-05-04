#include "WorldCube.hpp"
// #include <iostream>

WorldCube::WorldCube()
    : sideSize(2.0), sides{Side(Eigen::Vector3d::UnitX() * sideSize / 2.0,
                                -Eigen::Vector3d::UnitX()),
                           Side(Eigen::Vector3d::UnitX() * -sideSize / 2.0,
                                Eigen::Vector3d::UnitX()),
                           Side(Eigen::Vector3d::UnitY() * sideSize / 2.0,
                                -Eigen::Vector3d::UnitY()),
                           Side(Eigen::Vector3d::UnitY() * -sideSize / 2.0,
                                Eigen::Vector3d::UnitY()),
                           Side(Eigen::Vector3d::UnitZ() * sideSize / 2.0,
                                -Eigen::Vector3d::UnitZ()),
                           Side(Eigen::Vector3d::UnitZ() * -sideSize / 2.0,
                                Eigen::Vector3d::UnitZ())}
{
    const size_t numSpheres = 40;
    const double sphereSize = 0.2;

    const double initialDist = 0.02;

    const size_t steps = sideSize / (sphereSize + initialDist) - 1;

    spheres.reserve(numSpheres);

    auto toCoord = [&](size_t step) {
        return -sideSize / 2.0 + initialDist + (step + 1) * (sphereSize + 0.02);
    };

    for (size_t x = 0; x < steps; x++)
    {
        const double curX = toCoord(x);
        for (size_t y = 0; y < steps; y++)
        {
            const double curY = toCoord(y);
            for (size_t z = 0; z < steps; z++)
            {
                const double curZ = toCoord(z);
                spheres.emplace_back(Eigen::Vector3d(curX, curY, curZ),
                                     Eigen::Vector3d(0.05, 0.05, 0.1),
                                     sphereSize);

//                 std::cout << spheres.back().getPos().transpose() << std::endl;

                if (spheres.size() > numSpheres)
                {
                    break;
                }
            }
            if (spheres.size() > numSpheres)
            {
                break;
            }
        }
        if (spheres.size() > numSpheres)
        {
            break;
        }
    }
}

void WorldCube::updateObjects()
{
    for (Sphere &s : spheres)
    {
        const Eigen::Vector3d curVel = s.getPos() - s.getLastPos();
        const double velDist = curVel.norm();
        //         std::cout << "Initial pos : " << s.getPos().transpose() << "
        //         last Pos : " << s.getLastPos().transpose() << " vel is : " <<
        //         curVel.transpose() << std::endl;

        // speed up !
        const double wantedVel = 0.006;
        Eigen::Vector3d acceleration(Eigen::Vector3d::Zero());
        if (std::abs(velDist - wantedVel) > 1e-7)
        {
            const Eigen::Vector3d velNorm = curVel.normalized();

            if (velNorm.isZero())
            {
                acceleration = -s.getPos().normalized() * wantedVel / dt;
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
        Eigen::Vector3d nextPos = s.getPos() + curVel + acceleration * dt * dt;

        s.setPos(nextPos);
        // std::cout << "NExt pos : " << nextPos.transpose() << std::endl;
    }
}

void WorldCube::checkSphereSphereCollisions()
{
    // solve collisions
    for (size_t i = 0; i < spheres.size(); i++)
    {
        Sphere &cur(spheres[i]);
        for (size_t j = i + 1; j < spheres.size(); j++)
        {
            Sphere &other(spheres[j]);
            const auto intersection = cur.computeExitDir(other);

            if (!intersection)
            {
                continue;
            }

            //             std::cout << "Intersection : " << i << " and " << j
            //             << " depth: " << intersection->depth << " exit dir "
            //             << intersection->exitDir.transpose() << std::endl;

            const double d2 = intersection->depth / 2.0;
            cur.setPos(cur.getPos() + intersection->exitDir * d2);
            other.setPos(other.getPos() - intersection->exitDir * d2);

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
        for (Sphere &s : spheres)
        {
            double distToPlane = side.signedDistance(s.getPos());
            if (distToPlane < s.getRadius())
            {
                // std::cout << "Collision with side !" << std::endl;

                Eigen::Vector3d vel = s.getPos() - s.getLastPos();
                double velDist = vel.norm();
                Eigen::Vector3d velNormalized(vel / velDist);

                // std::cout << "Cur VEl is : " << velDist << std::endl;

                Eigen::ParametrizedLine<double, 3> velLine(s.getPos(),
                                                           velNormalized);

                Eigen::Vector3d travelIntersectionOnSide =
                    velLine.intersectionPoint(side);

                Eigen::Vector3d curPosOnPlane = side.projection(s.getPos());

                // std::cout << "travelIntersectionOnSide : " <<
                // travelIntersectionOnSide.transpose() << std::endl; std::cout
                // << "curPosOnPlane : " << curPosOnPlane.transpose() <<
                // std::endl;

                const double intersectionDepth = s.getRadius() - distToPlane;
                double distBackToClearIntersection;
                if (curPosOnPlane.isApprox(travelIntersectionOnSide))
                {
                    // we don't bounce off at an angle
                    distBackToClearIntersection = intersectionDepth;
                }
                else
                {
                    const Eigen::Vector3d velProjectedOnSide =
                        travelIntersectionOnSide - curPosOnPlane;

                    double cosAngle =
                        velProjectedOnSide.normalized().dot(velNormalized);

                    distBackToClearIntersection = intersectionDepth / cosAngle;
                }

                // Now we compute the bounce off position...
                const Eigen::Vector3d nonIntersectingPos(
                    s.getPos() - velNormalized * distBackToClearIntersection);

                const Eigen::Vector3d newVelDir =
                    -(Eigen::AngleAxisd(M_PI, side.normal()) * velNormalized);

                double distBeforeBounce =
                    (nonIntersectingPos - s.getPos()).norm();

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
    for (Sphere &s : spheres)
    {
        Eigen::Vector3d pos = s.getPos();

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
        for (Sphere &s : spheres)
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
