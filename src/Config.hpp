#pragma once

#include <cstdint>

constexpr uint32_t WIDTH = 1200;
constexpr uint32_t HEIGHT = 1200;

constexpr uint32_t SPHERE_VERTICE_SECTORS = 48;
constexpr uint32_t SPHERE_VERTICE_STACKS = 32;
constexpr uint32_t SPHERE_VERTICES =
    (SPHERE_VERTICE_SECTORS + 1) * (SPHERE_VERTICE_STACKS + 1);
constexpr uint32_t SPHERE_INDICES =
    SPHERE_VERTICE_SECTORS * SPHERE_VERTICE_STACKS * 6;

constexpr uint32_t LIGHTS_AMOUNT = 3;

constexpr uint32_t GRID_HALF_EXTEND = 10;
constexpr uint32_t GRID_VERTEX_COUNT = (GRID_HALF_EXTEND * 2 + 1) * 4;