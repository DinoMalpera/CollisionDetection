#include <gtest/gtest.h>
#include <algorithm>
#include "../BoundingVolumeHierarchy.h"
#include "../Naive_Oct_Split.h"
#include "../Mesh_Face.h"

using namespace CoDet;

TEST( BoundingVolumeHierarchy_Tree_Construction, Single_Element )
{
    std::vector<Mesh_Face> mesh;
    mesh.emplace_back(
        Mesh_Face{
            Point{ 0,0,0},
            Point{ 0,0,1},
            Point{ 0,1,0} } );

    BoundingVolumeHierarchy bvh(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh,
            Naive_Oct_Split ) );

    const auto root = bvh.get_root();

    ASSERT_EQ( root.get_child_volumes().size(), 0u );
    ASSERT_TRUE( root.get_face()->vertices == mesh[0].vertices );
}

TEST( BoundingVolumeHierarchy_Tree_Construction, Two_Elements )
{
    std::vector<Mesh_Face> mesh;
    mesh.emplace_back(
        Mesh_Face{
            Point{ 0,0,0},
            Point{ 0,0,1},
            Point{ 0,1,0} } );
    mesh.emplace_back(
        Mesh_Face{
            Point{ 0,0,0},
            Point{ 0,0,1},
            Point{ 0,1,1} } );

    BoundingVolumeHierarchy bvh(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh,
            Naive_Oct_Split ) );

    const auto root = bvh.get_root();

    ASSERT_EQ( root.get_child_volumes().size(), 2u );

    const auto& children = root.get_child_volumes();

    ASSERT_EQ( children[0u].get_child_volumes().size(), 0u );
    ASSERT_EQ( children[1u].get_child_volumes().size(), 0u );

    const auto it1 =
        std::find_if(
            children.begin(),
            children.end(),
            [&mesh](const auto& c ){ return c.get_face()->vertices == mesh[0].vertices; } );
    ASSERT_TRUE( children.end() != it1 );
    const auto it2 =
        std::find_if(
            children.begin(),
            children.end(),
            [&mesh](const auto& c ){ return c.get_face()->vertices == mesh[1].vertices; } );
    ASSERT_TRUE( children.end() != it2 );
}

