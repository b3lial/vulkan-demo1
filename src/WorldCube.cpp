#include "WorldCube.hpp"

WorldCube::WorldCube()
{
    const size_t numSpheres = 10;
    const double sphereSize = 0.2;

    sideSize = 2;

    for (size_t i = 0; i < numSpheres; i++)
    {
        spheres.emplace_back(Eigen::Vector3d::Constant(-sideSize/2.0 + 0.02 + i * (sphereSize + 0.02)),
                             Eigen::Vector3d(0.5, 0.5, 0.1), sphereSize);
    }

}
void WorldCube::updateObjects()
{
    for (Sphere &s : spheres)
    {
        const Eigen::Vector3d curVel = s.getPos() - s.getLastPos();
        const double velDist = curVel.norm();
        //std::cout << "Initial pos : " << s.getPos().transpose() << " last Pos : " << s.getLastPos().transpose() << " vel is : " << curVel.transpose() << std::endl;

        //speed up !
        const double wantedVel = 0.02;
        Eigen::Vector3d acceleration(Eigen::Vector3d::Zero());
        if(std::abs(velDist - wantedVel) > 1e-7)
        {
            const Eigen::Vector3d velNorm = curVel.normalized();

            if(velNorm.isZero())
            {
                acceleration = -s.getPos().normalized() * wantedVel / dt;
                //std::cout << "Velocity is zero, setting acc : " << acceleration.transpose() << std::endl;
            }
            else
            {

                acceleration = (wantedVel - velDist) / dt * velNorm;
                //std::cout << "Velocity is " << velDist << " wanted : " << wantedVel  << " , setting acc : " << acceleration.transpose() << std::endl;
            }
        }

        // update for new iterration
        s.setLastPos();

        // verlet integrate
        Eigen::Vector3d nextPos = s.getPos() + curVel + acceleration * dt * dt;

        s.setPos(nextPos);
        //std::cout << "NExt pos : " << nextPos.transpose() << std::endl;
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

            //std::cout << "Intersection : " << i << " and " << j << " depth: " << intersection->depth << " exit dir " << intersection->exitDir.transpose() << std::endl;

            const double d2 = intersection->depth / 2.0;
            cur.setPos(cur.getPos() + intersection->exitDir * d2);
            other.setPos(other.getPos() - intersection->exitDir * d2);

            //std::cout << "New Pos  : " << cur.getPos().transpose() << " and " << other.getPos().transpose() << std::endl;
        }
    }
}

void WorldCube::checkSphereCubeCollisions()
{
    using Side = Eigen::Hyperplane<double, 3>;

    const std::array<Side, 6> sides{
        Side(Eigen::Vector3d::UnitX() * sideSize / 2.0, -Eigen::Vector3d::UnitX()),
        Side(Eigen::Vector3d::UnitX() * -sideSize / 2.0, Eigen::Vector3d::UnitX()),
        Side(Eigen::Vector3d::UnitY() * sideSize / 2.0, -Eigen::Vector3d::UnitY()),
        Side(Eigen::Vector3d::UnitY() * -sideSize / 2.0, Eigen::Vector3d::UnitY()),
        Side(Eigen::Vector3d::UnitZ() * sideSize / 2.0, -Eigen::Vector3d::UnitZ()),
        Side(Eigen::Vector3d::UnitZ() * -sideSize / 2.0, Eigen::Vector3d::UnitZ())
    };

    for(const Side &side : sides)
    {
        for (Sphere &s : spheres)
        {
            double distToPlane = side.signedDistance(s.getPos());
            if (distToPlane < s.getSize())
            {
                //std::cout << "Collision with side !" << std::endl;

                Eigen::Vector3d vel = s.getPos() - s.getLastPos();
                double velDist = vel.norm();
                Eigen::Vector3d velNormalized(vel / velDist);

                //std::cout << "Cur VEl is : " << velDist << std::endl;

                Eigen::ParametrizedLine<double, 3> velLine(s.getPos(), velNormalized);

                Eigen::Vector3d travelIntersectionOnSide = velLine.intersectionPoint(side);

                Eigen::Vector3d curPosOnPlane = side.projection(s.getPos());

                //std::cout << "travelIntersectionOnSide : " << travelIntersectionOnSide.transpose() << std::endl;
                //std::cout << "curPosOnPlane : " << curPosOnPlane.transpose() << std::endl;

                const double intersectionDepth = s.getSize() - distToPlane;
                double distBackToClearIntersection;
                if(curPosOnPlane.isApprox(travelIntersectionOnSide))
                {
                    // we don't bounce off at an angle
                    distBackToClearIntersection = intersectionDepth;
                }
                else
                {
                    const Eigen::Vector3d velProjectedOnSide = travelIntersectionOnSide - curPosOnPlane;

                    double cosAngle = velProjectedOnSide.normalized().dot(velNormalized);

                    distBackToClearIntersection = intersectionDepth / cosAngle;
                }

                //Now we compute the bounce off position...
                const Eigen::Vector3d nonIntersectingPos(s.getPos() - velNormalized * distBackToClearIntersection);

                const Eigen::Vector3d newVelDir = -(Eigen::AngleAxisd(M_PI, side.normal()) * velNormalized);

                double distBeforeBounce = (nonIntersectingPos - s.getPos()).norm();

                s.setPos(nonIntersectingPos + newVelDir * (velDist - distBeforeBounce));

                //weired hack, preserve velocity by modifying the last pos
                s.setLastPos(nonIntersectingPos + newVelDir * -distBeforeBounce);

                //std::cout << "New Pos  : " << s.getPos().transpose() << " new last pos : " << s.getLastPos().transpose() << std::endl;
            }
        }
    }

    // HACK never leave the cube
    for(Sphere &s : spheres)
    {
        Eigen::Vector3d pos = s.getPos();

        for(size_t i = 0; i < 3; i++)
        {
            pos[i] = std::min(sideSize / 2.0 - s.getSize(), pos[i]);
            pos[i] = std::max(-sideSize / 2.0 - s.getSize(), pos[i]);
        }

        s.setPos(pos);
    }
}


void WorldCube::stepWorld()
{
    //std::cout <<std::endl << "NEW STEP !" << std::endl;

    for (Sphere &s : spheres)
    {
        const Eigen::Vector3d curVel = s.getPos() - s.getLastPos();
        //std::cout << "Initial pos : " << s.getPos().transpose() << " last Pos : " << s.getLastPos().transpose() << " vel is : " << curVel.transpose() << std::endl;
    }


    checkSphereCubeCollisions();
    checkSphereSphereCollisions();

    updateObjects();



}
