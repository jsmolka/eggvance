#include "common/macros.h"
#include "common/utility.h"

template<int limit>
inline void WindowDimension<limit>::reset()
{
    *this = {};
}

template<int limit>
inline u8 WindowDimension<limit>::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");
    return 0;

}

template<int limit>
inline void WindowDimension<limit>::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");
    bytes[index] = byte;

    max = bytes[0];
    min = bytes[1];

    if (max > limit || max < min)
        max = limit;
}

template<int limit>
inline bool WindowDimension<limit>::contains(int x) const
{
    return x >= min && x < max;
}
