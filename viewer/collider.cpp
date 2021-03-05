#include "collider.h"

constexpr double kEpsilon = std::numeric_limits<float>::epsilon();

bool IsIntersecting(
	const vec3& v0,
	const vec3& v1,
	const vec3& v2,
	const vec3& org,
	const vec3& dir,
	float& dist,
	bool enable_culling)
{
	vec3 v01 = v1 - v0;
	vec3 v02 = v2 - v0;
	vec3 pvc = cross(dir, v02); // T
	float det = dot(v01, pvc); // ((P1, V02, V01))

	if (enable_culling && det < 0.f) return false;
	if (!enable_culling && fabs(det) < kEpsilon) return false;

	float inv = 1 / det;
	vec3 tvc = org - v0; // P0 - V0
	float u = dot(tvc, pvc) * inv; // Eq.3
	if (u < 0.0f || u > 1.0f) return false;

	vec3 qvc = cross(tvc, v01); // S
	float v = dot(dir, qvc) * inv; // Eq.4
	if (v < 0.0f || u + v > 1.0f) return false;

	// distance from ray.origin to hit point
	float t = dot(v02, qvc) * inv; // Eq.5

	// update hit distance
	if (t > 0.0f && dist > t)
	{
		dist = t;
		return true; // ray hit primitive in distance
	}
	else return false; // ray hit primitive out of distance
}

OpenMesh::FaceHandle Collider::collide(
	const vec3& org,
	const vec3& dir,
	float& dist) const
{
	Primitive ret;
	PrimitiveTriangle triangle(*pMesh);
	int numIntersectPri = 0;

	for (Primitive vF : pMesh->faces())
	{
		vec3 v0, v1, v2;
		triangle(vF, v0, v1, v2);

		if (++numIntersectPri && IsIntersecting(v0, v1, v2, org, dir, dist))
		{
			ret = vF;
		}
	}

	printf("Number of Primitive intersecting test = %d\n", numIntersectPri);

	return ret;
}

Primitive Collider::collide(
	const Bvh& bvh,
	const vec3& org,
	const vec3& dir,
	float& dist) const
{
	PrimitiveTriangle triangle(*pMesh);
	PrimitiveCollide collide(triangle);
	bvh.Intersect(collide, org, dir, dist);
	return collide.closest;
}
