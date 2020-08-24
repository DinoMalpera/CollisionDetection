#include <gtest/gtest.h>
#include "../Pairwise_Pruning.h"
#include "../BoundingVolumeHierarchy.h"
#include "../Naive_Oct_Split.h"
#include "../Mesh_Face.h"

using namespace CoDet;

using mesh_t = std::vector<Mesh_Face>;

class Pair_Checker
{
private:
    const pairwise_pruning_return_t&    collision_candidate_faces;
    const mesh_t&                       mesh1;
    const mesh_t&                       mesh2;
public:
    Pair_Checker(
            const pairwise_pruning_return_t&    collision_candidate_faces,
            const mesh_t&                       mesh1,
            const mesh_t&                       mesh2
    )
        :   collision_candidate_faces    (collision_candidate_faces)
        ,   mesh1                        (mesh1)
        ,   mesh2                        (mesh2)
    {}
public:
    void
    operator()(
        const unsigned int face1_index,
        const unsigned int face2_index ) const
    {
        const auto it =
            std::find_if(
                collision_candidate_faces.begin(),
                collision_candidate_faces.end(),
                [this,face1_index,face2_index]
                (const auto& pair)
                {
                    return
                          (mesh1[face1_index] == *std::get<0u>(pair))
                        & (mesh2[face2_index] == *std::get<1u>(pair));
                });
        ASSERT_TRUE( it != collision_candidate_faces.end() );
    }
};

static
auto
translate_face(
        const Mesh_Face&    face,
        const Point&        p )
{
    Mesh_Face ret(face);

    ret.vertices[0u] = ret.vertices[0u] + p;
    ret.vertices[1u] = ret.vertices[1u] + p;
    ret.vertices[2u] = ret.vertices[2u] + p;

    return ret;
}

TEST( Pairwise_Pruning, Distant_Meshes )
{
    const mesh_t mesh1{
        Mesh_Face{
            Point{ 0,0,0},
            Point{ 0,0,1},
            Point{ 0,1,0} } };

    BoundingVolumeHierarchy bvh1(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh1,
            Naive_Oct_Split ) );

    mesh_t mesh2;
    mesh2.emplace_back(
        Mesh_Face{
            Point{ 5,0,0},
            Point{ 5,0,1},
            Point{ 5,1,0} } );
    BoundingVolumeHierarchy bvh2(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh2,
            Naive_Oct_Split ) );


    const auto collision_candidate_faces = pairwise_pruning( bvh1, bvh2 );

    ASSERT_EQ( collision_candidate_faces.size(), 0u );
}

TEST( Pairwise_Pruning, Single_Face_Collision )
{
    const mesh_t mesh1{
        Mesh_Face{
            Point{ 0,0,0},
            Point{ 0,0,1},
            Point{ 1,1,0} } };
    BoundingVolumeHierarchy bvh1(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh1,
            Naive_Oct_Split ) );

    const mesh_t mesh2{
        Mesh_Face{
            Point{ 1,0,0},
            Point{ 1,0,1},
            Point{ 0,1,0} } };
    BoundingVolumeHierarchy bvh2(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh2,
            Naive_Oct_Split ) );


    const auto collision_candidate_faces = pairwise_pruning( bvh1, bvh2 );

    ASSERT_EQ( collision_candidate_faces.size(), 1u );

    const Pair_Checker check_pair(
        collision_candidate_faces,
        mesh1,
        mesh2 );

    check_pair( 0u, 0u );
}

TEST( Pairwise_Pruning, NearMiss_Collision )
{
    const auto face1 =
        Mesh_Face{
            Point{ 0,0,0},
            Point{ 0,0,1},
            Point{ 1,1,0} };

    const mesh_t mesh1{
        translate_face( face1, Point{0, 3,0} ),
        translate_face( face1, Point{0,-2,0} ),
        translate_face( face1, Point{0, 0,3} ) };
    BoundingVolumeHierarchy bvh1(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh1,
            Naive_Oct_Split ) );

    const mesh_t mesh2{
        face1 };
    BoundingVolumeHierarchy bvh2(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh2,
            Naive_Oct_Split ) );


    const auto collision_candidate_faces = pairwise_pruning( bvh1, bvh2 );

    ASSERT_EQ( collision_candidate_faces.size(), 0u );
}

TEST( Pairwise_Pruning, SingleFace_vs_LargeObject_Collision )
{
    const auto face1 =
        Mesh_Face{
            Point{ 0,0,0},
            Point{ 0,0,1},
            Point{ 1,1,0} };
    const auto face2 =
        Mesh_Face{
            Point{ 1,0,0},
            Point{ 1,0,1},
            Point{ 0,1,0} };

    const mesh_t mesh1{
        translate_face( face1, Point{0, 3,0} ),
        translate_face( face1, Point{0,-2,0} ),
                        face2,
        translate_face( face1, Point{0, 0,3} ) };
    BoundingVolumeHierarchy bvh1(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh1,
            Naive_Oct_Split ) );

    const mesh_t mesh2{
        face1 };
    BoundingVolumeHierarchy bvh2(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh2,
            Naive_Oct_Split ) );


    const auto collision_candidate_faces = pairwise_pruning( bvh1, bvh2 );

    ASSERT_EQ( collision_candidate_faces.size(), 1u );

    const Pair_Checker check_pair(
        collision_candidate_faces,
        mesh1,
        mesh2 );

    check_pair( 2u, 0u );
}

TEST( Pairwise_Pruning, SingleFace_collides_with_multiple_faces )
{
    const auto face1 =
        Mesh_Face{
            Point{ 0,0,0},
            Point{ 0,0,1},
            Point{ 1,1,0} };
    const auto face2 =
        Mesh_Face{
            Point{ 1,0,0},
            Point{ 1,0,1},
            Point{ 0,1,0} };
    const auto face3 =
        Mesh_Face{
            Point{ 0.5,0,0},
            Point{ 0.5,0,1},
            Point{ 0,1,0} };

    const mesh_t mesh1{
        translate_face( face1, Point{0, 3,0} ),
        translate_face( face1, Point{0,-2,0} ),
                        face2,
        translate_face( face1, Point{0, 0,3} ),
                        face3 };
    BoundingVolumeHierarchy bvh1(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh1,
            Naive_Oct_Split ) );

    const mesh_t mesh2{
        face1 };
    BoundingVolumeHierarchy bvh2(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh2,
            Naive_Oct_Split ) );


    const auto collision_candidate_faces = pairwise_pruning( bvh1, bvh2 );

    ASSERT_EQ( collision_candidate_faces.size(), 2u );

    const Pair_Checker check_pair(
        collision_candidate_faces,
        mesh1,
        mesh2 );

    check_pair( 2u, 0u );
    check_pair( 4u, 0u );
}

TEST( Pairwise_Pruning, MultipleFaces_Collision )
{
    const auto face1 =
        Mesh_Face{
            Point{ 0,0,0},
            Point{ 0,0,1},
            Point{ 1,1,0} };
    const auto face2 =
        Mesh_Face{
            Point{ 1,0,0},
            Point{ 1,0,1},
            Point{ 0,1,0} };
    const auto face3 =
        Mesh_Face{
            Point{ 0.5,0,0},
            Point{ 0.5,0,1},
            Point{ 0,1,0} };

    const mesh_t mesh1{
                        face1,
        translate_face( face1, {  2, 0, 0 } ),
        translate_face( face1, { -2, 0, 0 } ),
        translate_face( face1, {  0, 0, 2 } ),
        translate_face( face1, {  0, 0,-2 } ),
        translate_face( face1, {  2, 0, 2 } ),
        translate_face( face1, { -2, 0, 2 } ) };
    BoundingVolumeHierarchy bvh1(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh1,
            Naive_Oct_Split ) );

    const mesh_t mesh2{
                        face2,
        translate_face( face2, {  0, 2, 0 } ),
        translate_face( face2, {  0,-2, 0 } ),
        translate_face( face2, {  2, 0, 0 } ),
        translate_face( face3, {  0, 0,-2 } ),
        translate_face( face2, { -2, 2,-2 } ) };
    BoundingVolumeHierarchy bvh2(
        BoundingVolumeHierarchy::make_BVH_topDown<decltype(Naive_Oct_Split)>(
            mesh2,
            Naive_Oct_Split ) );


    const auto collision_candidate_faces = pairwise_pruning( bvh1, bvh2 );

    ASSERT_EQ( collision_candidate_faces.size(), 3u );

    const Pair_Checker check_pair(
        collision_candidate_faces,
        mesh1,
        mesh2 );

    check_pair( 0u, 0u );
    check_pair( 1u, 3u );
    check_pair( 4u, 4u );
}

