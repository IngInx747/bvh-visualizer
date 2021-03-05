#pragma once
#ifndef COLLIDER_H
#define COLLIDER_H

#include "Math.h"
#include "Mesh.h"
#include "bvh.h"

class Collider
{
public:
    Collider() {}

    Collider(TheMesh* _pMesh) { set_mesh(_pMesh); }

    void set_mesh(TheMesh* _pMesh) { pMesh = _pMesh; }

    void unset_mesh() { pMesh = NULL; }

    Primitive collide(
        const vec3& org,
        const vec3& dir,
        float& dist) const;

    Primitive collide(
        const Bvh& bvh,
        const vec3& org,
        const vec3& dir,
        float& dist) const;

protected:
    TheMesh* pMesh = NULL;
};

bool IsIntersecting(
    const vec3& v0,
    const vec3& v1,
    const vec3& v2,
    const vec3& org,
    const vec3& dir,
    float& dist,
    bool enable_culling = false);

#endif // !COLLIDER_H
