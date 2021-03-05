#pragma once
#ifndef BOUNDING_VOLUME_HIERARCHY_H
#define BOUNDING_VOLUME_HIERARCHY_H

#include <memory>

#include "aabb.h"
#include "Mesh.h"

struct BvhNode;
struct PrimitiveBound;
struct PrimitiveSplit;

using Primitive = OpenMesh::FaceHandle;

// When representing inner node, i0, i1 = index of left and right child nodes
// in the node array respectively;
// When representing leaf node, i0, i1 = beginning index of object in the
// primitive array and NEGATIVE number of objects.
struct BvhNode
{
	Aabb bbox;
	int i0 = 0;
	int i1 = 0;
};

struct PrimitiveBound
{
	typedef OpenMesh::VertexHandle   VertexHandle;
	typedef OpenMesh::HalfedgeHandle HalfedgeHandle;
	typedef OpenMesh::EdgeHandle     EdgeHandle;
	typedef OpenMesh::FaceHandle     FaceHandle;

	Aabb operator() (const Primitive& hF) const;

	PrimitiveBound(const TheMesh& mesh) : mesh(mesh) {}

	const TheMesh& mesh;
};

struct PrimitiveSplit
{
	int operator() (std::vector<Primitive>& primitives, int beginId, int endId) const;

	PrimitiveSplit(const PrimitiveBound& bound) : bound(bound) {}

	const PrimitiveBound& bound;
};

struct PrimitiveTriangle
{
	typedef OpenMesh::VertexHandle   VertexHandle;
	typedef OpenMesh::HalfedgeHandle HalfedgeHandle;
	typedef OpenMesh::EdgeHandle     EdgeHandle;
	typedef OpenMesh::FaceHandle     FaceHandle;

	void operator()(const Primitive& p, vec3& v0, vec3& v1, vec3& v2) const;

	PrimitiveTriangle(const TheMesh& mesh) : mesh(mesh) {}

	const TheMesh& mesh;
};

struct PrimitiveCollide
{
	bool operator() (const Primitive& primitive, const vec3& org, const vec3& dir, float& dist) const;

	PrimitiveCollide(const PrimitiveTriangle& tri) : triangle(tri) {}

	const PrimitiveTriangle& triangle;
	mutable Primitive closest;
	bool culling = 1;  // 0 for ray tracing; 1 for picking triangle
};

class Bvh
{
public:
	void Build(
		const std::vector<Primitive>& primitives,
		const PrimitiveBound& bound,
		const PrimitiveSplit& split,
		int numObjPerNode = 1);

	bool Intersect(
		const PrimitiveCollide& collide,
		const vec3& org,
		const vec3& dir,
		float& dist) const;

	std::vector<BvhNode>& GetNodes() { return mNodes; }
	const std::vector<BvhNode>& GetNodes() const { return mNodes; }

	std::vector<Primitive>& GetPrimitives() { return mPrimitives; }
	const std::vector<Primitive>& GetPrimitives() const { return mPrimitives; }

	//const Aabb& GetRootBox() const { assert(mNodes.size() > 0 && mNodes[0]); return mNodes[0]->bbox; }

protected:
	void BuildRecursive(
		int beginId,
		int endId,
		int nodeId,
		int depth,
		const PrimitiveBound& bound,
		const PrimitiveSplit& split);

protected:
	std::vector<Primitive> mPrimitives;
	std::vector<BvhNode> mNodes;
	int mThreshold = 1;
};

inline int& Left(BvhNode& node) { return node.i0; }
inline const int& Left(const BvhNode& node) { return node.i0; }
inline int& Right(BvhNode& node) { return node.i1; }
inline const int& Right(const BvhNode& node) { return node.i1; }
inline int& Offset(BvhNode& node) { return node.i0; }
inline const int& Offset(const BvhNode& node) { return node.i0; }
inline int& NegLen(BvhNode& node) { return node.i1; }
inline const int& NegLen(const BvhNode& node) { return node.i1; }
inline int Length(const BvhNode& node) { return -node.i1; }

inline void SetLeaf(BvhNode& node, int objIdx, int objNum)
{
	Offset(node) = objIdx;
	NegLen(node) = -objNum;
}

inline bool IsLeaf(const BvhNode& node)
{
	return NegLen(node) < 0;
}

#endif