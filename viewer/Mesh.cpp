#include "Mesh.h"

using namespace OpenMesh;
using M = TheMesh;
using Point = M::Point;

void TheMethod::set_mesh(TheMesh* _pMesh)
{
    pMesh = _pMesh;
    _assign_properties();
}

void TheMethod::unset_mesh()
{
    _release_properties();
    pMesh = NULL;
}

int TheMethod::_assign_properties()
{
    int retval = 0;

    // Default properties

    if (!pMesh->has_face_normals())
    {
        pMesh->request_face_normals();
        if (!pMesh->has_face_normals())
        {
            fprintf(stderr, "ERROR: Allocation of property 'face normals' failed!\n");
            retval = 1;
        }
    }

    if (!pMesh->has_vertex_normals())
    {
        pMesh->request_vertex_normals();
        if (!pMesh->has_vertex_normals())
        {
            fprintf(stderr, "ERROR: Allocation of property 'vertex normals' failed!\n");
            retval = 1;
        }
    }

    // Customized properties

    pMesh->add_property(eprop_sharp_);

    return retval;
}

int TheMethod::_release_properties()
{
    int retval = 0;

    // Default properties

    if (pMesh->has_face_normals())
    {
        pMesh->release_face_normals();
        if (pMesh->has_face_normals())
        {
            fprintf(stderr, "ERROR: Release of property 'face normals' failed!\n");
            retval = 1;
        }
    }

    if (pMesh->has_vertex_normals())
    {
        pMesh->release_vertex_normals();
        if (pMesh->has_vertex_normals())
        {
            fprintf(stderr, "ERROR: Release of property 'vertex normals' failed!\n");
            retval = 1;
        }
    }

    // Customized properties

    pMesh->remove_property(eprop_sharp_);

    return retval;
}

void resize_unit_box(TheMesh& _mesh)
{
    Point s(0, 0, 0);

    for (M::VertexIter viter = _mesh.vertices_begin(); viter != _mesh.vertices_end(); ++viter)
    {
        VertexHandle hV = *viter;
        s = s + _mesh.point(hV);
    }
    s = s / _mesh.n_vertices();

    for (M::VertexIter viter = _mesh.vertices_begin(); viter != _mesh.vertices_end(); ++viter)
    {
        VertexHandle hV = *viter;
        Point p = _mesh.point(hV);
        p = p - s;
        _mesh.set_point(hV, p);
    }

    float d = 0;
    for (M::VertexIter viter = _mesh.vertices_begin(); viter != _mesh.vertices_end(); ++viter)
    {
        VertexHandle hV = *viter;
        Point p = _mesh.point(hV);
        for (int k = 0; k < 3; k++)
        {
            d = (d > fabs(p[k])) ? d : fabs(p[k]);
        }
    }

    for (M::VertexIter viter = _mesh.vertices_begin(); viter != _mesh.vertices_end(); ++viter)
    {
        VertexHandle hV = *viter;
        Point p = _mesh.point(hV);
        p = p / d;
        _mesh.set_point(hV, p);
    }
}
