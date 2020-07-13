#pragma once

#include <vector>

namespace CoDet {

// forward decls
class BoundingVolumeHierarchy;
struct Mesh_Face;

// defines
using pairwise_pruning_return_t = std::vector<std::tuple<const Mesh_Face*,const Mesh_Face*>>;

// THE pruning function
pairwise_pruning_return_t
pairwise_pruning(
        const BoundingVolumeHierarchy&    bvh1,
        const BoundingVolumeHierarchy&    bvh2 );

}

