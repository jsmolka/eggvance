#pragma once

#include <climits>

#include "base/int.h"

namespace detail
{

template<uint Size, uint Mask, uint Init, uint Type, uint Unsigned>
class RegisterBase
{
public:
    using Value = eggcpt::stdint_t<Size, Unsigned>;

    static constexpr uint kSize = Size;
    static constexpr uint kMask = Mask;
    static constexpr uint kInit = Init;
    static constexpr uint kType = Type;

    RegisterBase() = default;
    RegisterBase(Value value)
        : value(value) {}

    template<uint Index>
    u8 read() const
    {
        static_assert(Index < Size);
        static_assert(Type & 0x1);

        return data[Index];
    }

    template<uint Index, uint WriteMask = 0xFFFF'FFFF>
    void write(u8 byte)
    {
        static_assert(Index < Size);
        static_assert(Type & 0x2);

        constexpr auto kMask = static_cast<u8>((Mask & WriteMask) >> (CHAR_BIT * Index)); 

        data[Index] = byte & kMask;
    }

    union
    {
        u8 data[Size];
        Value value = Init;
    };
};

}  // namespace detail

template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, uint Unsigned = 1> using RegisterR = detail::RegisterBase<Size, Mask, Init, 0x1, Unsigned>;
template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, uint Unsigned = 1> using RegisterW = detail::RegisterBase<Size, Mask, Init, 0x2, Unsigned>;
template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, uint Unsigned = 1> using Register  = detail::RegisterBase<Size, Mask, Init, 0x3, Unsigned>;
