#pragma once

#include "point.h"

struct Dimensions
{
    constexpr Dimensions()
        : Dimensions(0, 0) 
    {

    }

    constexpr Dimensions(int w, int h)
        : w(w)
        , h(h) 
    {
    
    }

    constexpr bool contains(const Point& point) const
    {
        return point.x >= 0
            && point.y >= 0
            && point.x < w
            && point.y < h;
    }

    constexpr Dimensions operator+(int scalar) const
    {
        return Dimensions(
            w + scalar,
            h + scalar
        );
    }

    constexpr Dimensions operator-(int scalar) const
    {
        return Dimensions(
            w - scalar,
            h - scalar
        );
    }

    constexpr Dimensions operator*(int scalar) const
    {
        return Dimensions(
            w * scalar,
            h * scalar
        );
    }

    constexpr Dimensions operator/(int scalar) const
    {
        return Dimensions(
            w / scalar,
            h / scalar
        );
    }

    constexpr Dimensions operator%(int scalar) const
    {
        return Dimensions(
            w % scalar,
            h % scalar
        );
    }

    constexpr Dimensions operator<<(int scalar) const
    {
        return Dimensions(
            w << scalar,
            h << scalar
        );
    }

    constexpr Dimensions operator>>(int scalar) const
    {
        return Dimensions(
            w >> scalar,
            h >> scalar
        );
    }

    constexpr Dimensions operator+(const Dimensions& other) const
    {
        return Dimensions(
            w + other.w,
            h + other.h
        );
    }

    constexpr Dimensions operator-(const Dimensions& other) const
    {
        return Dimensions(
            w - other.w,
            h - other.h
        );
    }

    constexpr Dimensions operator*(const Dimensions& other) const
    {
        return Dimensions(
            w * other.w,
            h * other.h
        );
    }

    constexpr Dimensions operator/(const Dimensions& other) const
    {
        return Dimensions(
            w / other.w,
            h / other.h
        );
    }

    constexpr Dimensions operator%(const Dimensions& other) const
    {
        return Dimensions(
            w % other.w,
            h % other.h
        );
    }

    constexpr Dimensions operator<<(const Dimensions& other) const
    {
        return Dimensions(
            w << other.w,
            h << other.h
        );
    }

    constexpr Dimensions operator>>(const Dimensions& other) const
    {
        return Dimensions(
            w >> other.w,
            h >> other.h
        );
    }

    constexpr Dimensions& operator+=(int scalar)
    {
        w += scalar;
        h += scalar;

        return *this;
    }

    constexpr Dimensions& operator-=(int scalar)
    {
        w -= scalar;
        h -= scalar;

        return *this;
    }

    constexpr Dimensions& operator*=(int scalar)
    {
        w *= scalar;
        h *= scalar;

        return *this;
    }

    constexpr Dimensions& operator/=(int scalar)
    {
        w /= scalar;
        h /= scalar;

        return *this;
    }

    constexpr Dimensions& operator%=(int scalar)
    {
        w %= scalar;
        h %= scalar;

        return *this;
    }

    constexpr Dimensions& operator<<=(int scalar)
    {
        w <<= scalar;
        h <<= scalar;

        return *this;
    }

    constexpr Dimensions& operator>>=(int scalar)
    {
        w >>= scalar;
        h >>= scalar;

        return *this;
    }

    constexpr Dimensions& operator+=(const Dimensions& other)
    {
        w += other.w;
        h += other.h;

        return *this;
    }

    constexpr Dimensions& operator-=(const Dimensions& other)
    {
        w -= other.w;
        h -= other.h;

        return *this;
    }

    constexpr Dimensions& operator*=(const Dimensions& other)
    {
        w *= other.w;
        h *= other.h;

        return *this;
    }

    constexpr Dimensions& operator/=(const Dimensions& other)
    {
        w /= other.w;
        h /= other.h;

        return *this;
    }

    constexpr Dimensions& operator%=(const Dimensions& other)
    {
        w %= other.w;
        h %= other.h;

        return *this;
    }

    constexpr Dimensions& operator<<=(const Dimensions& other)
    {
        w <<= other.w; 
        h <<= other.h;

        return *this;
    }

    constexpr Dimensions& operator>>=(const Dimensions& other)
    {
        w >>= other.w;
        h >>= other.h;

        return *this;
    }

    int w;
    int h;
};

constexpr Point operator+(const Point& point, const Dimensions& dims)
{
    return Point(
        point.x + dims.w,
        point.y + dims.h
    );
}
