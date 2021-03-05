#include "bvh.h"

#include <stack>

#include "collider.h" // IsIntersecting(...)
#include "viewer.h"

//struct PrimitiveBound
//{
//	typedef OpenMesh::VertexHandle   VertexHandle;
//	typedef OpenMesh::HalfedgeHandle HalfedgeHandle;
//	typedef OpenMesh::EdgeHandle     EdgeHandle;
//	typedef OpenMesh::FaceHandle     FaceHandle;
//	typedef OpenMesh::Vec3f  Point;
//	typedef OpenMesh::Vec3f  Normal;
//	typedef OpenMesh::Vec2f  TexCoord;
//	typedef OpenMesh::Vec3uc Color;
//
//	Aabb operator() (const Primitive& hF) const
//	{
//		HalfedgeHandle hH = mesh.halfedge_handle(hF);
//		VertexHandle hV0 = mesh.to_vertex_handle(hH);
//		hH = mesh.next_halfedge_handle(hH);
//		VertexHandle hV1 = mesh.to_vertex_handle(hH);
//		hH = mesh.next_halfedge_handle(hH);
//		VertexHandle hV2 = mesh.to_vertex_handle(hH);
//
//		Point v0 = mesh.point(hV0);
//		Point v1 = mesh.point(hV1);
//		Point v2 = mesh.point(hV2);
//
//		return Bound(
//			v0[0], v0[1], v0[2],
//			v1[0], v1[1], v1[2],
//			v2[0], v2[1], v2[2]);
//	}
//
//	PrimitiveBound(const TheMesh& mesh) : mesh(mesh) {}
//
//	const TheMesh& mesh;
//};

Aabb PrimitiveBound::operator() (const Primitive& hF) const
{
	HalfedgeHandle hH = mesh.halfedge_handle(hF);
	VertexHandle hV0 = mesh.to_vertex_handle(hH);
	hH = mesh.next_halfedge_handle(hH);
	VertexHandle hV1 = mesh.to_vertex_handle(hH);
	hH = mesh.next_halfedge_handle(hH);
	VertexHandle hV2 = mesh.to_vertex_handle(hH);

	Point v0 = mesh.point(hV0);
	Point v1 = mesh.point(hV1);
	Point v2 = mesh.point(hV2);

	return Bound(
		v0[0], v0[1], v0[2],
		v1[0], v1[1], v1[2],
		v2[0], v2[1], v2[2]);
}

//struct PrimitiveSplit
//{
//	int operator() (std::vector<Primitive>& primitives, int beginId, int endId) const
//	{
//		Aabb cbox = Bound(); // centroid bounding box
//		for (const Primitive& p : primitives)
//			cbox = Union(cbox, bound(p));
//		int dim = GetMaxExtentDim(cbox);
//		float mid = (cbox.pMin + cbox.pMax)[dim];
//
//		auto pPtr = std::partition(
//			primitives.begin() + beginId,
//			primitives.begin() + endId,
//			[&](const Primitive& p) { return GetCentroid(bound(p))[dim] < mid; });
//
//		return std::distance(primitives.begin(), pPtr);
//	}
//
//	PrimitiveSplit(const PrimitiveBound& bound) : bound(bound) {}
//
//	const PrimitiveBound& bound;
//};

#if 0
// Split Method: Middle
// Partition primitives through node's midpoint
int PrimitiveSplit::operator() (std::vector<Primitive>& primitives, int beginId, int endId) const
{
	auto beginIter = primitives.begin() + beginId;
	auto endIter = primitives.begin() + endId;
	Aabb cbox = Bound(); // centroid bounding box

	for (auto iter = beginIter; iter != endIter; ++iter)
		cbox = Union(cbox, bound(*iter));

	int dim = GetMaxExtentDim(cbox);
	float mid = (cbox.pMin[dim] + cbox.pMax[dim]) * 0.5f;

	auto pIter = std::partition(beginIter, endIter,
		[&](const Primitive& p) { return GetCentroid(bound(p))[dim] < mid; });

	return std::distance(primitives.begin(), pIter);
}
#else
// Split Method: EqualCounts
// Partition primitives into equally-sized subsets
int PrimitiveSplit::operator() (std::vector<Primitive>& primitives, int beginId, int endId) const
{
	auto beginIter = primitives.begin() + beginId;
	auto endIter = primitives.begin() + endId;
	Aabb cbox = Bound(); // centroid bounding box

	for (auto iter = beginIter; iter != endIter; ++iter)
		cbox = Union(cbox, bound(*iter));

	int dim = GetMaxExtentDim(cbox);
	int mid = (beginId + endId) / 2;
	auto midIter = primitives.begin() + mid;

	std::nth_element(beginIter, midIter, endIter,
		[&](const Primitive& a, const Primitive& b)
	{ return GetCentroid(bound(a))[dim] < GetCentroid(bound(b))[dim]; });

	return mid;
}
#endif

void PrimitiveTriangle::operator()(const Primitive& hF, vec3& v0, vec3& v1, vec3& v2) const
{
	HalfedgeHandle hH = mesh.halfedge_handle(hF);
	VertexHandle hV0 = mesh.to_vertex_handle(hH);
	hH = mesh.next_halfedge_handle(hH);
	VertexHandle hV1 = mesh.to_vertex_handle(hH);
	hH = mesh.next_halfedge_handle(hH);
	VertexHandle hV2 = mesh.to_vertex_handle(hH);

	Point p0 = mesh.point(hV0);
	Point p1 = mesh.point(hV1);
	Point p2 = mesh.point(hV2);

	v0 = { p0[0], p0[1], p0[2] };
	v1 = { p1[0], p1[1], p1[2] };
	v2 = { p2[0], p2[1], p2[2] };
}

bool PrimitiveCollide::operator()(const Primitive& primitive, const vec3& org, const vec3& dir, float& dist) const
{
	vec3 v0, v1, v2;
	triangle(primitive, v0, v1, v2);
	bool hit = IsIntersecting(v0, v1, v2, org, dir, dist, culling);
	if (hit) closest = primitive;
	return hit;
}

void Bvh::Build(
	const std::vector<Primitive>& primitives,
	const PrimitiveBound& bound,
	const PrimitiveSplit& split,
	int numObjPerNode)
{
	std::copy(primitives.begin(), primitives.end(), std::back_inserter(mPrimitives));

	mThreshold = numObjPerNode;
	mNodes.emplace_back();

	BuildRecursive(0, mPrimitives.size(), 0, 0, bound, split);
}

void Bvh::BuildRecursive(
	int beginId,
	int endId,
	int nodeId,
	int depth,
	const PrimitiveBound& bound,
	const PrimitiveSplit& split)
{
	if ((endId - beginId) <= mThreshold)
	{
		SetLeaf(mNodes[nodeId], beginId, endId - beginId);
		Aabb bbox = Bound();
		for (int i = beginId; i < endId; ++i)
			bbox = Union(bbox, bound(mPrimitives[i]));
		mNodes[nodeId].bbox = bbox;
	}
	else
	{
		// Split primitives into left and right children nodes at splitting index
		int splitId = split(mPrimitives, beginId, endId);

		// Make leaf node if it failed to split primitives into 2 sets
		if (splitId == beginId || splitId == endId)
		{
			SetLeaf(mNodes[nodeId], beginId, endId - beginId);
			Aabb bbox = Bound();
			for (int i = beginId; i < endId; ++i)
				bbox = Union(bbox, bound(mPrimitives[i]));
			mNodes[nodeId].bbox = bbox;
		}
		// Build Bvh recursively after splitting primitives
		else
		{
			mNodes[nodeId].bbox = Bound();

			int left = static_cast<int>(mNodes.size());
			Left(mNodes[nodeId]) = left;
			mNodes.emplace_back();

			BuildRecursive(beginId, splitId, left, depth + 1, bound, split);
			mNodes[nodeId].bbox = Union(mNodes[nodeId].bbox, mNodes[left].bbox);

			int right = static_cast<int>(mNodes.size());
			Right(mNodes[nodeId]) = right;
			mNodes.emplace_back();

			BuildRecursive(splitId, endId, right, depth + 1, bound, split);
			mNodes[nodeId].bbox = Union(mNodes[nodeId].bbox, mNodes[right].bbox);
		}
	}
}

bool Bvh::Intersect(
	const PrimitiveCollide& collide,
	const vec3& org,
	const vec3& dir,
	float& dist) const
{
	bool hit = false;
	std::stack<int> recursive;
	int curr = 0;
	vec3 invDir = { 1.f / dir.x, 1.f / dir.y, 1.f / dir.z };
	int3 isNeg = { dir.x < 0, dir.y < 0, dir.z < 0 };

	// _DEBUG
	int numIntersectBox = 0;
	int numIntersectPri = 0;

	while (true)
	{
		const BvhNode& node = mNodes[curr]; // safe

		//if (++numIntersectBox && IsIntersecting(curr->bbox, org, dir, dist))
		if (++numIntersectBox && IsIntersecting(node.bbox, org, invDir, dist, true))
		{
			draw_aabb(node.bbox); // _DEBUG

			if (IsLeaf(node))
			{
				int beginId = Offset(node);
				int endId = Offset(node) + Length(node);

				for (int i = beginId; i < endId; ++i)
					if (++numIntersectPri && collide(mPrimitives[i], org, dir, dist))
						hit = true;

				if (recursive.empty()) break;
				curr = recursive.top();
				recursive.pop();
			}
			else
			{
				int dim = GetMaxExtentDim(node.bbox);
				
				if (isNeg[dim])
				{
					recursive.push(Left(node));
					curr = Right(node);
				}
				else
				{
					recursive.push(Right(node));
					curr = Left(node);
				}
			}
		}
		else
		{
			if (recursive.empty()) break;
			curr = recursive.top();
			recursive.pop();
		}
	}

	// _DEBUG
	printf("Number of AABB intersecting test = %d\n", numIntersectBox);
	printf("Number of Primitive intersecting test = %d\n", numIntersectPri);
	printf("Intersecting test result = %d\n", hit);

	return hit;
}
