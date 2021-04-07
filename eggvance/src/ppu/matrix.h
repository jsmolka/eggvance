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

    void fill(const T& value)
    {
        for (auto& column : *this)
        {
            column.fill(value);
        }
    }
};

class RotationScalingMatrix : public Matrix<s16, 2, 2>
{
public:
    Point operator*(const Point& point) const;

    void write(uint index, s16 half);
};

inline constexpr RotationScalingMatrix kIdentityMatrix = { 1 << kDecimalBits, 0, 0, 1 << kDecimalBits };

class TransformationMatrix : public Matrix<s32, 2, 3>
{
public:
    TransformationMatrix();

    Point operator*(s32 x) const;

    void writeA(uint index, u8 byte);
    void writeB(uint index, u8 byte);
    void writeC(uint index, u8 byte);
    void writeD(uint index, u8 byte);
    void writeX(uint index, u8 byte);
    void writeY(uint index, u8 byte);

    void hblank();
    void vblank();

private:
    s32 yx = 0;
    s32 yy = 0;
};
