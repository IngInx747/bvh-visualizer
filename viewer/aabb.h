#pragma once
#ifndef AABB_H
#define AABB_H

#include "Math.h"

#define AABB_AXIS_X 0
#define AABB_AXIS_Y 1
#define AABB_AXIS_Z 2

struct Aabb
{
	vec3 pMin, pMax;
};

#include "aabb.hpp"

#endif // !AABB_H
