#pragma once

#include <algorithm>

namespace CoDet {

using float_t = double;

/*  Helper that applies the same procedure
*   to every coordinate of the argument. */
template<typename Clo>
inline
void
for_each_coordinate( Clo clo)
{
    clo(0u);
    clo(1u);
    clo(2u);
}

/*  Given vector of elements generate
*   vector of pointers to those elements. */
template<typename T>
inline
std::vector<const T*>
generate_vector_of_pointers_to_elements(
        const std::vector<T>& src )
{
    std::vector<const T*> dst( src.size() );
    std::transform(
        src.begin(),
        src.end(),
        dst.begin(),
        []
        ( const auto& el )
        {
            return &el;
        } );

    return dst;
}

}

