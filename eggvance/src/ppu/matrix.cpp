#include "matrix.h"

#include "base/bit.h"

Point RotationScalingMatrix::operator*(const Point& point) const
{
    Point result(
        get(0, 0) * point.x + get(0, 1) * point.y,
        get(1, 0) * point.x + get(1, 1) * point.y);

    return result >> kDecimalBits;
}

void RotationScalingMatrix::write(uint index, s16 half)
{
    set((index >> 1) & 0x1, index & 0x1, half);
}

TransformationMatrix::TransformationMatrix()
{
    fill(0);

    set(0, 0, 0x0100);
    set(1, 1, 0x0100);
}

Point TransformationMatrix::operator*(s32 x) const
{
    Point result(
        get(0, 0) * x + get(0, 1) * yx + get(0, 2),
        get(1, 0) * x + get(1, 1) * yy + get(1, 2));

    return result >> kDecimalBits;
}

void TransformationMatrix::writeA(uint index, u8 byte)
{
    auto value = static_cast<s16>(get(0, 0));

    bit::byteRef(value, index) = byte;

    set(0, 0, static_cast<s32>(value));
}

void TransformationMatrix::writeB(uint index, u8 byte)
{
    auto value = static_cast<s16>(get(0, 1));

    bit::byteRef(value, index) = byte;

    set(0, 1, static_cast<s32>(value));
}

void TransformationMatrix::writeC(uint index, u8 byte)
{
    auto value = static_cast<s16>(get(1, 0));

    bit::byteRef(value, index) = byte;

    set(1, 0, static_cast<s32>(value));
}

void TransformationMatrix::writeD(uint index, u8 byte)
{
    auto value = static_cast<s16>(get(1, 1));

    bit::byteRef(value, index) = byte;

    set(1, 1, static_cast<s32>(value));
}

void TransformationMatrix::writeX(uint index, u8 byte)
{
    if (index == 3)
        byte = bit::signEx<4>(byte & 0xF);

    bit::byteRef(get(0, 2), index) = byte;

    yx = 0;
}

void TransformationMatrix::writeY(uint index, u8 byte)
{
    if (index == 3)
        byte = bit::signEx<4>(byte & 0xF);

    bit::byteRef(get(1, 2), index) = byte;

    yy = 0;
}

void TransformationMatrix::hblank()
{
    yx++;
    yy++;
}

void TransformationMatrix::vblank()
{
    yx = 0;
    yy = 0;
}
