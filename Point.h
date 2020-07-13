#pragma once

#include <array>
#include "common_utils.h"

namespace CoDet {

struct Point
{
public:
    std::array<float_t,3u> data;
};

template <typename F>
inline
Point
make_point(
        F f )
{
    Point p;

    for_each_coordinate(
        [&p,f]
        ( const unsigned int coord )
        {
            p.data[coord] = f( coord );
        } );

    return p;
}

inline
Point
operator+(
        const Point& l,
        const Point& r )
{
    return
        make_point(
            [&l,&r]
            ( const unsigned int coord )
            {
                return l.data[coord] + r.data[coord];
            } );
}

inline
Point
operator*(
        const Point&    l,
        const float_t   f )
{
    return
        make_point(
            [&l,f]
            ( const unsigned int coord )
            {
                return l.data[coord] * f;
            } );
}

/*    used for testing */
inline
bool
operator==(
        const Point& l,
        const Point& r )
{
    return
        l.data == r.data;
}

}

