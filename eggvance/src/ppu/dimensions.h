#pragma once

#include "point.h"

struct Dimensions
{
    constexpr Dimensions()
        : Dimensions(0, 0) { };

    constexpr Dimensions(int w, int h)
        : w(w), h(h) { }

    constexpr bool contains(const Point& p) const
    {
        return p.x >= 0
            && p.y >= 0
            && p.x < w
            && p.y < h;
    }

    constexpr Dimensions operator+(int scalar) const
    {
        return { w + scalar, h + scalar };
    }

    constexpr Dimensions operator-(int scalar) const
    {
        return { w - scalar, h - scalar };
    }

    constexpr Dimensions operator*(int scalar) const
    {
        return { w * scalar, h * scalar };
    }

    constexpr Dimensions operator/(int scalar) const
    {
        return { w / scalar, h / scalar };
    }

    constexpr Dimensions operator%(int scalar) const
    {
        return { w % scalar, h % scalar };
    }

    constexpr Dimensions operator+(const Dimensions& other) const
    {
        return { w + other.w, h + other.h };
    }

    constexpr Dimensions operator-(const Dimensions& other) const
    {
        return { w - other.w, h - other.h };
    }

    constexpr Dimensions operator*(const Dimensions& other) const
    {
        return { w * other.w, h * other.h };
    }

    constexpr Dimensions operator/(const Dimensions& other) const
    {
        return { w / other.w, h / other.h };
    }

    constexpr Dimensions operator%(const Dimensions& other) const
    {
        return { w % other.w, h % other.h };
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

    int w = 0;
    int h = 0;
};
