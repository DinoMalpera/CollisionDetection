#pragma once

#include "Point.h"
#include <array>

namespace CoDet {

/*  Holds mesh face data. */
struct Mesh_Face
{
public:
    std::array<Point,3u>    vertices;
};

/*    used for testing */
inline
bool
operator==(
        const Mesh_Face& lhs,
        const Mesh_Face& rhs )
{
    return
        lhs.vertices == rhs.vertices;
}

}

