#include <limits>
#include <tuple>
#include <cassert>
#include "BoundingVolumeHierarchy.h"
#include "Mesh_Face.h"
#include "BBox.h"
#include "Naive_Oct_Split.h"

using namespace CoDet;

namespace {

/*  Compute Bounding Box
*/
BBox
find_bounds(
        const Mesh_face_iterator    mesh_face_data_begin,
        const Mesh_face_iterator    mesh_face_data_end )
{
    assert( mesh_face_data_begin < mesh_face_data_end );

    constexpr float max_val = std::numeric_limits<float_t>::max();
    BBox bbox{  { +max_val, +max_val, +max_val},
                { -max_val, -max_val, -max_val} };

    for( auto face=mesh_face_data_begin; face!=mesh_face_data_end; ++face )
    {
        for( const auto& vert : (*face)->vertices )
        {
            for_each_coordinate(
                [&bbox, vert]
                ( const unsigned int coord )
                {
                    if ( bbox.max.data[coord] < vert.data[coord] ) bbox.max.data[coord] = vert.data[coord];
                    if ( bbox.min.data[coord] > vert.data[coord] ) bbox.min.data[coord] = vert.data[coord];
                });
        }
    }

    assert( +max_val != bbox.max.data[0u] );
    assert( -max_val != bbox.min.data[0u] );

    return bbox;
}

/*  Build BVH using top down approach.
    Partitioning method is provided as a policy.
*/
template <typename Partitioning_policy>
auto
_build_BVH_topDown(
        const Mesh_face_iterator    mesh_face_data_begin,
        const Mesh_face_iterator    mesh_face_data_end,
        Partitioning_policy            partitioning_policy )
{
    assert( mesh_face_data_begin < mesh_face_data_end );

    const auto bbox = find_bounds( mesh_face_data_begin, mesh_face_data_end );

    const auto number_of_faces = mesh_face_data_end - mesh_face_data_begin;

    assert( 0u != number_of_faces );
    if ( 1u == number_of_faces )
    {
        return
            BVH_Node::make_BVH_Node(
                mesh_face_data_begin,
                bbox );
    }

    // partition and sort faces and get ranges for each partition
    const auto child_volume_ranges =
        partitioning_policy(
            mesh_face_data_begin,
            mesh_face_data_end,
            bbox );

    assert( 0u != child_volume_ranges.size() );

    std::vector<BVH_Node> ret;
    // produce a node for each partition
    for( auto& child_volume_range : child_volume_ranges )
    {
        ret.emplace_back(
            _build_BVH_topDown<Partitioning_policy>(
                std::get<0>( child_volume_range ),
                std::get<1>( child_volume_range ),
                partitioning_policy ) );
    }

    return
        BVH_Node::make_BVH_Node(
            std::move( ret ),
            bbox );
}

}

template <typename Partitioning_policy>
BoundingVolumeHierarchy
BoundingVolumeHierarchy::make_BVH_topDown(
        const std::vector<Mesh_Face>&   mesh_face_data,
        Partitioning_policy             partitioning_policy )
{
    std::vector<const Mesh_Face*> mesh_face_data_sortable
        = generate_vector_of_pointers_to_elements(
            mesh_face_data );

    return
        BoundingVolumeHierarchy(
            _build_BVH_topDown<Partitioning_policy>(
                mesh_face_data_sortable.begin(),
                mesh_face_data_sortable.end(),
                partitioning_policy ) );
}

//    explicit template instantiation
template
BoundingVolumeHierarchy
BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
        const std::vector<Mesh_Face>&,
        decltype(Naive_Oct_Split) );

