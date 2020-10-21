#pragma once

class Point
{
public:
    constexpr Point() = default;

    constexpr Point(int x, int y)
        : x(x)
        , y(y) {}

    constexpr int index2d(int width) const
    {
        return width * y + x;
    }

    constexpr bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    constexpr bool operator!=(const Point& other) const { return x != other.x || y != other.y; }
    constexpr bool operator< (const Point& other) const { return x <  other.x && y <  other.y; }
    constexpr bool operator> (const Point& other) const { return x >  other.x && y >  other.y; }
    constexpr bool operator<=(const Point& other) const { return x <= other.x && y <= other.y; }
    constexpr bool operator>=(const Point& other) const { return x >= other.x && y >= other.y; }

    #define POINT_OPERATOR(op)                                 \
        constexpr Point operator op(int scalar) const          \
        {                                                      \
            return Point(x op scalar, y op scalar);            \
        }                                                      \
                                                               \
        constexpr Point operator op(const Point& other) const  \
        {                                                      \
            return Point(x op other.x, y op other.y);          \
        }                                                      \
                                                               \
        constexpr Point& operator op##=(int scalar)            \
        {                                                      \
            x op##= scalar;                                    \
            y op##= scalar;                                    \
            return *this;                                      \
        }                                                      \
                                                               \
        constexpr Point& operator op##=(const Point& other)    \
        {                                                      \
            x op##= other.x;                                   \
            y op##= other.y;                                   \
            return *this;                                      \
        }

    POINT_OPERATOR(+)
    POINT_OPERATOR(-)
    POINT_OPERATOR(*)
    POINT_OPERATOR(/)
    POINT_OPERATOR(%)
    POINT_OPERATOR(^)
    POINT_OPERATOR(|)
    POINT_OPERATOR(&)
    POINT_OPERATOR(<<)
    POINT_OPERATOR(>>)

    #undef POINT_OPERATOR

    int x = 0;
    int y = 0;
};
