#pragma once

struct Point
{
    constexpr Point() = default;

    constexpr Point(int x, int y)
        : x(x), y(y) {}

    constexpr int index2d(int width) const
    {
        return width * y + x;
    }

    #define OPERATOR(op)                                        \
        constexpr Point operator##op(int scalar) const          \
        {                                                       \
            return Point(x ##op scalar, y ##op scalar);         \
        }                                                       \
                                                                \
        constexpr Point operator##op(const Point& other) const  \
        {                                                       \
            return Point(x ##op other.x, y ##op other.y);       \
        }                                                       \
                                                                \
        constexpr Point& operator##op##=(int scalar)            \
        {                                                       \
            x ##op##= scalar;                                   \
            y ##op##= scalar;                                   \
            return *this;                                       \
        }                                                       \
                                                                \
        constexpr Point& operator##op##=(const Point& other)    \
        {                                                       \
            x ##op##= other.x;                                  \
            y ##op##= other.y;                                  \
            return *this;                                       \
        }

    OPERATOR(+)
    OPERATOR(-)
    OPERATOR(*)
    OPERATOR(/)
    OPERATOR(%)
    OPERATOR(<<)
    OPERATOR(>>)

    #undef OPERATOR

    int x = 0;
    int y = 0;
};

inline constexpr Point kOrigin(0, 0);
