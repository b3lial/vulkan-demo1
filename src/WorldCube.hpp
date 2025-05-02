#pragma once

#include "Sphere.hpp"
#include <vector>

class WorldCube
{
public:
    WorldCube();


    void stepWorld();

private:

    double sideSize;

    std::vector<Sphere> spheres;



};
