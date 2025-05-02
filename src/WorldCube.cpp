#include "WorldCube.hpp"

WorldCube::WorldCube()
{
    const size_t numSpheres = 20;
    const double sphereSize = 0.2;

    for(size_t i = 0; i < numSpheres; i++)
    {
        spheres.emplace_back(Eigen::Vector3d::Constant(i), Eigen::Vector3d::UnitX(), sphereSize);
    }

    sideSize = 5;

}


void WorldCube::stepWorld()
{
    const double dt = 0.01;
    for(Sphere &s : spheres)
    {
        Eigen::Vector3d curVel = s.getPos() - s.getLastPos();

        //update for new iterration
        s.setLastPos();

        // stupid forward euler
        Eigen::Vector3d nextPos = s.getPos() + curVel * dt;

        s.setPos(nextPos);
    }

    //solve collisions
    for(size_t i = 0 ; i  < spheres.size();i++)
    {
        Sphere &cur(spheres[i]);
        for(size_t j = i+ 1; j < spheres.size(); j++)
        {
            Sphere &other(spheres[i]);
            const auto intersection = cur.computeExitDir(other);

            if(!intersection)
            {
                continue;
            }

            const double d2 = intersection->depth / 2.0;
            cur.setPos(cur.getPos() + intersection->exitDir * d2);
            other.setPos(other.getPos() - intersection->exitDir * d2);
        }
    }

    //Collide with box
    Eigen::Hyperplane<double, 3> side(Eigen::Vector3d::UnitX() * sideSize / 2.0, -Eigen::Vector3d::UnitX());

    for(Sphere &s : spheres)
    {
        double distToPlane = side.signedDistance(s.getPos());
        if(distToPlane < s.getSize())
        {
            Eigen::Vector3d vel = s.getPos() - s.getLastPos();
            double velDist = vel.norm();

            //FIXME this is wrong, but I am to stoned to do math...

        }
    }

}
