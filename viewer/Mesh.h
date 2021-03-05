#pragma once
#ifndef MESH_H
#define MESH_H

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Geometry/VectorT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Attributes.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Mesh/IteratorsT.hh>
#include <OpenMesh/Core/Mesh/Status.hh >

// Static attributes(traits)

struct TheTraits : public OpenMesh::DefaultTraits
{
    typedef OpenMesh::Vec3f Point;
    typedef OpenMesh::Vec3f Normal;
    typedef OpenMesh::Vec2f TexCoord;
    typedef OpenMesh::Vec3uc Color;

    // Default attributes

    VertexAttributes(OpenMesh::Attributes::Status);

    HalfedgeAttributes(OpenMesh::Attributes::None);

    EdgeAttributes(OpenMesh::Attributes::Status);

    FaceAttributes(OpenMesh::Attributes::Status);

    // Customized attributes

    VertexTraits{};

    HalfedgeTraits{};

    EdgeTraits{};

    FaceTraits{};
};

// General mesh definition

typedef OpenMesh::TriMesh_ArrayKernelT<TheTraits> TheMesh;

void resize_unit_box(TheMesh& _mesh);

// Dynamic attributes(properties)

class TheMethod
{
public:
    TheMethod() {}

    TheMethod(TheMesh* _pMesh)
    {
        set_mesh(_pMesh);
    }

    ~TheMethod()
    {
        unset_mesh();
    }

    // set mesh reference and assign properties for the method
    void set_mesh(TheMesh* _pMesh);

    // release mesh reference and restore the mesh by clearing properties
    void unset_mesh();

    TheMesh& mesh() { return *pMesh; }
    const TheMesh& mesh() const { return *pMesh; }

private:
    // assign any properties necessary for the method
    int _assign_properties();

    // release the properties assigned previously
    int _release_properties();

private:
    // reference to mesh
    TheMesh *pMesh = NULL;
    OpenMesh::MeshHandle hMesh_;

    // edge property: sharp
    OpenMesh::EPropHandleT<int> eprop_sharp_;
};

#endif // !MESH_H
