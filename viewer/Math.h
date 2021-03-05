#pragma once
#ifndef MATH_H
#define MATH_H

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <OpenMesh/Core/Geometry/VectorT.hh>

using vec3 = glm::vec3;
using int3 = glm::int3;
using Point = OpenMesh::Vec3f;

inline vec3 o2g(const Point& p) { return{ p[0],p[1],p[2] }; }
inline Point g2o(const vec3& p) { return{ p[0],p[1],p[2] }; }

static constexpr float MachineEpsilon = std::numeric_limits<float>::epsilon() * 0.5f;
inline constexpr float gamma(int n) { return (n * MachineEpsilon) / (1 - n * MachineEpsilon); }

#endif // !MATH_H
