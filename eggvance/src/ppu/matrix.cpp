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
    setByte<16>(0, 0, index, byte);
}

void TransformationMatrix::writeB(uint index, u8 byte)
{
    setByte<16>(0, 1, index, byte);
}

void TransformationMatrix::writeC(uint index, u8 byte)
{
    setByte<16>(1, 0, index, byte);
}

void TransformationMatrix::writeD(uint index, u8 byte)
{
    setByte<16>(1, 1, index, byte);
}

void TransformationMatrix::writeX(uint index, u8 byte)
{
    setByte<28>(0, 2, index, byte);
    yx = 0;
}

void TransformationMatrix::writeY(uint index, u8 byte)
{
    setByte<28>(1, 2, index, byte);
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

template<uint kSize>
void TransformationMatrix::setByte(uint x, uint y, uint index, u8 byte)
{
    auto& value = get(x, y);
    bit::byteRef(value, index) = byte;
    value = bit::signEx<kSize>(value);
}
