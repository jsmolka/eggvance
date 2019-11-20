#pragma once

struct Point
{
    constexpr Point(int x, int y)
        : x(x), y(y) { }

    constexpr int offset(int width) const
    {
        return width * y + x;
    }

    constexpr Point operator+(int scalar) const
    {
        return { x + scalar, y + scalar };
    }

    constexpr Point operator-(int scalar) const
    {
        return { x - scalar, y - scalar };
    }

    constexpr Point operator*(int scalar) const
    {
        return { x * scalar, y * scalar };
    }

    constexpr Point operator/(int scalar) const
    {
        return { x / scalar, y / scalar };
    }

    constexpr Point operator%(int scalar) const
    {
        return { x % scalar, y % scalar };
    }

    constexpr Point operator+(const Point& other) const
    {
        return { x + other.x, y + other.y };
    }

    constexpr Point operator-(const Point& other) const
    {
        return { x - other.x, y - other.y };
    }

    constexpr Point operator*(const Point& other) const
    {
        return { x * other.x, y * other.y };
    }

    constexpr Point operator/(const Point& other) const
    {
        return { x / other.x, y / other.y };
    }

    constexpr Point operator%(const Point& other) const
    {
        return { x % other.x, y % other.y };
    }

    constexpr Point& operator+=(int scalar)
    {
        x += scalar;
        y += scalar;

        return *this;
    }

    constexpr Point& operator-=(int scalar)
    {
        x -= scalar;
        y -= scalar;

        return *this;
    }

    constexpr Point& operator*=(int scalar)
    {
        x *= scalar;
        y *= scalar;

        return *this;
    }

    constexpr Point& operator/=(int scalar)
    {
        x /= scalar;
        y /= scalar;

        return *this;
    }

    constexpr Point& operator%=(int scalar)
    {
        x %= scalar;
        y %= scalar;

        return *this;
    }

    constexpr Point& operator+=(const Point& other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    constexpr Point& operator-=(const Point& other)
    {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    constexpr Point& operator*=(const Point& other)
    {
        x *= other.x;
        y *= other.y;

        return *this;
    }

    constexpr Point& operator/=(const Point& other)
    {
        x /= other.x;
        y /= other.y;

        return *this;
    }

    constexpr Point& operator%=(const Point& other)
    {
        x %= other.x;
        y %= other.y;

        return *this;
    }

    int x;
    int y;
};
