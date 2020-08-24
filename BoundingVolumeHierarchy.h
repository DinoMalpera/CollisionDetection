#pragma once

#include "BVH_Node.h"

namespace CoDet {

struct Mesh_Face;

/*  BVH class
*
*   Entry point to the bounding volume
*   hierarchy.
*/
class BoundingVolumeHierarchy final
{
private:
    BVH_Node    root;

public:
    const BVH_Node&
    get_root() const
    {
        return root;
    }

private:
    BoundingVolumeHierarchy(
            const BVH_Node& root_node
    )
        :   root    (root_node)
    {}

public:
    template <typename Partitioning_policy>
    static
    BoundingVolumeHierarchy
    make_BVH_topDown(
            const std::vector<Mesh_Face>&   mesh_face_data,
            Partitioning_policy             partitioning_policy );
};

}

