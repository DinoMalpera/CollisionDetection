#include "BoundingVolumeHierarchy.h"
#include "Pairwise_Pruning.h"
#include <tuple>

using namespace CoDet;

namespace {

/*  Check whether two bounding boxes intersect
*/
bool
do_bbox_intersect(
        const BBox bbox1,
        const BBox bbox2 )
{
    assert( bbox1.min.data[0] <= bbox1.max.data[0] );
    assert( bbox1.min.data[1] <= bbox1.max.data[1] );
    assert( bbox1.min.data[2] <= bbox1.max.data[2] );
    assert( bbox2.min.data[0] <= bbox2.max.data[0] );
    assert( bbox2.min.data[1] <= bbox2.max.data[1] );
    assert( bbox2.min.data[2] <= bbox2.max.data[2] );

    const auto test_coordinate_intersection =
        [&bbox1,&bbox2]
        ( const unsigned int coord )
        {
            return
                  (bbox1.min.data[coord] < bbox2.max.data[coord])
                & (bbox1.max.data[coord] > bbox2.min.data[coord]);    // bitwise operator to remove dependency
        };

    return
          test_coordinate_intersection(0u)
        & test_coordinate_intersection(1u)
        & test_coordinate_intersection(2u);    // bitwise operator to remove dependency
}

/*    If interior node, return its children.
 *    If leaf, return vector comprised only of that leaf node.
 * */
auto
get_vector_of_candidates(
        const BVH_Node*const node )
{
    assert( nullptr != node );
    assert(    (!node->get_child_volumes().empty() && nullptr == node->get_face())
            || ( node->get_child_volumes().empty() && nullptr != node->get_face()) );

    if ( !node->get_child_volumes().empty() )
    {
        return
            generate_vector_of_pointers_to_elements(
                node->get_child_volumes() );
    }

    return std::vector<const BVH_Node*>{node};
}

bool
both_candidates_are_leaf_nodes(
        const BVH_Node& node1_child,
        const BVH_Node& node2_child )
{
    return
          (node1_child.get_child_volumes().empty())
        & (node2_child.get_child_volumes().empty());    // bitwise operator to remove dependency
}

}

pairwise_pruning_return_t
CoDet::pairwise_pruning(
        const BoundingVolumeHierarchy&    bvh1,
        const BoundingVolumeHierarchy&    bvh2 )
{
    pairwise_pruning_return_t                                   candidate_faces;
    std::vector<std::tuple<const BVH_Node*,const BVH_Node*>>    candidates;
    std::vector<std::tuple<const BVH_Node*,const BVH_Node*>>    new_candidates;

    const auto& root1 = bvh1.get_root();
    const auto& root2 = bvh2.get_root();

    const auto bbox1 = root1.get_bbox();
    const auto bbox2 = root2.get_bbox();

    if ( false == do_bbox_intersect( bbox1, bbox2 ) )
    {
        return candidate_faces;
    }

    candidates.emplace_back( &root1, &root2 );

    //    Take all current candidates and test their children against each other.
    //    For those pairs of children where bboxes do intersect, add them to the
    //    pool of candidates.

    while( !candidates.empty() )
    {
        for( const auto& candidate_pair : candidates )
        {
            const auto& node1 = std::get<0u>( candidate_pair );
            const auto& node2 = std::get<1u>( candidate_pair );

            assert( nullptr != node1 );
            assert( nullptr != node2 );

            const auto& node1_candidates =
                get_vector_of_candidates( node1 );

            const auto& node2_candidates =
                get_vector_of_candidates( node2 );

            assert( !node1_candidates.empty() );
            assert( !node2_candidates.empty() );

            for( const auto& node1_child : node1_candidates )
            for( const auto& node2_child : node2_candidates )
            {
                if ( do_bbox_intersect( node1_child->get_bbox(), node2_child->get_bbox() ) )
                {
                    if ( both_candidates_are_leaf_nodes( *node1_child, *node2_child ) )
                    {
                        candidate_faces.emplace_back(
                            node1_child->get_face(),
                            node2_child->get_face() );
                    } else {
                        new_candidates.emplace_back(
                            node1_child,
                            node2_child );
                    }
                }
            }
        }

        new_candidates.swap( candidates );
        new_candidates.clear();
    }

    return candidate_faces;
}

