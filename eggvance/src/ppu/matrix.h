#pragma once

#include <shell/array.h>
#include <shell/macros.h>

#include "constants.h"
#include "point.h"
#include "base/int.h"

template<typename T, std::size_t kRows, std::size_t kCols>
class Matrix : public shell::array<T, kRows, kCols>
{
public:
    static_assert(kRows > 0);
    static_assert(kCols > 0);

    constexpr std::size_t rows() const
    {
        return kRows;
    }

    constexpr std::size_t cols() const
    {
        return kCols;
    }

    const T& get(std::size_t x, std::size_t y) const
    {
        SHELL_ASSERT(x < kRows);
        SHELL_ASSERT(y < kCols);

        return (*this)[x][y];
    }

    T& get(std::size_t x, std::size_t y)
    {
        SHELL_ASSERT(x < kRows);
        SHELL_ASSERT(y < kCols);

        return (*this)[x][y];
    }

    void set(std::size_t x, std::size_t y, const T& value)
    {
        SHELL_ASSERT(x < kRows);
        SHELL_ASSERT(y < kCols);

        (*this)[x][y] = value;
    }
};

class RotationScalingMatrix : public Matrix<s16, 2, 2>
{
public:
    Point operator*(const Point& point) const;
};

inline constexpr RotationScalingMatrix kIdentityMatrix = { 1 << kDecimalBits, 0, 0, 1 << kDecimalBits };
