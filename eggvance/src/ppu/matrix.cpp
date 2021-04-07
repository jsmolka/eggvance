#include "matrix.h"

Point RotationScalingMatrix::operator*(const Point& point) const
{
    Point result(
        get(0, 0) * point.x + get(0, 1) * point.y,
        get(1, 0) * point.x + get(1, 1) * point.y);

    return result >> kDecimalBits;
}
