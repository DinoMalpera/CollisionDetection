#pragma once

#include <vector>
#include <cassert>
#include "BBox.h"

namespace CoDet {

struct Mesh_Face;
using Mesh_face_iterator = decltype( std::vector<const Mesh_Face*>().begin() );

/*  Node of the BVH.
*
*   Holds bounding box of elements contained.
*   It also holds either child nodes or mesh faces (if leaf).
*/
class BVH_Node final
{
private:
    BBox                    bbox;
    std::vector<BVH_Node>   child_volumes;
    Mesh_Face*              face;

private:
    explicit
    BVH_Node(
            const Mesh_face_iterator    mesh_face_data,
            const BBox                  bbox
    )
        :   bbox    (bbox)
        ,   face    (const_cast<Mesh_Face*>(*mesh_face_data))
    {
    }
private:
    explicit
    BVH_Node(
            std::vector<BVH_Node>&&      nodes,
            const BBox                   bbox
    )
        :   bbox            (bbox)
        ,   child_volumes   (nodes)
        ,   face            (nullptr)
    {
        assert( !child_volumes.empty() );
    }

public:
    const std::vector<BVH_Node>&
    get_child_volumes() const
    {
        return child_volumes;
    }
public:
    const Mesh_Face*
    get_face() const
    {
        return face;
    }
public:
    const BBox&
    get_bbox() const
    {
        return bbox;
    }

public:
    static
    auto
    make_BVH_Node(
            const Mesh_face_iterator    mesh_face_data,
            const BBox                  bbox )
    {
        return
            BVH_Node(
                mesh_face_data,
                bbox );
    }
public:
    static
    auto
    make_BVH_Node(
            std::vector<BVH_Node>&& nodes,
            const BBox                bbox )
    {
        return
            BVH_Node(
                std::move(nodes),
                bbox );
    }
};

}

