#pragma once

#include <algorithm>
#include <tuple>
#include <vector>
#include <cassert>
#include "BBox.h"
#include "BVH_Node.h"

namespace CoDet {

template <unsigned int coord, typename T>
inline
auto
partition_by_coord(
        const T        it0,
        const T        it1,
        const float_t  midpoint )
{
    assert( it0 <= it1 );

    return
        std::partition(
            it0,
            it1,
            [midpoint]
            ( const auto& el )
            {
                return
                    (
                        (el->vertices[0u].data[coord]
                        +el->vertices[1u].data[coord]
                        +el->vertices[2u].data[coord])
                        * 0.33333333333
                    ) < midpoint;
            } );
}

/*    Partition faces using bisection on every coordinate for a maximum of 8 partitions
 *     and a minimum of two (assuming reasonable input).
 * */
inline
auto
Naive_Oct_Split(
        const Mesh_face_iterator    mesh_face_data_begin,
        const Mesh_face_iterator    mesh_face_data_end,
        const BBox                  bbox )
{
    assert( mesh_face_data_begin < mesh_face_data_end );

    const auto midpoint = (bbox.min + bbox.max) * 0.5f;

    std::vector<std::tuple<const Mesh_face_iterator,const Mesh_face_iterator>> ret;
    Mesh_face_iterator it[9u];

    it[0u] = mesh_face_data_begin;
    it[8u] = mesh_face_data_end;

    it[4u] = partition_by_coord<0u>( it[0u], it[8u], midpoint.data[0u] );

    it[2u] = partition_by_coord<1u>( it[0u], it[4u], midpoint.data[1u] );
    it[6u] = partition_by_coord<1u>( it[4u], it[8u], midpoint.data[1u] );

    it[1u] = partition_by_coord<2u>( it[0u], it[2u], midpoint.data[2u] );
    it[3u] = partition_by_coord<2u>( it[2u], it[4u], midpoint.data[2u] );
    it[5u] = partition_by_coord<2u>( it[4u], it[6u], midpoint.data[2u] );
    it[7u] = partition_by_coord<2u>( it[6u], it[8u], midpoint.data[2u] );

    for( unsigned int child=1u; child<9u; ++child )
    {
        assert( it[child] >= it[child-1u] );

        if ( 0u != (it[child] - it[child-1u]) )
        {
            ret.emplace_back( it[child-1u], it[child] );
        }
    }

    return ret;
}

}

