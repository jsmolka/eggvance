#pragma once

#include <climits>
#include <type_traits>

#include "base/eggcpt.h"

namespace detail
{
    template<uint Size, uint Mask, uint Init, uint Type, typename T>
    struct RegisterBase
    {
        static_assert(Size == sizeof(T));

        static constexpr uint kSize = Size;
        static constexpr uint kMask = Mask;
        static constexpr uint kInit = Init;
        static constexpr uint kType = Type;

        virtual ~RegisterBase() = default;

        template<uint Index>
        u8 read() const
        {
            static_assert(Type & 0x1);
            static_assert(Index < Size);

            return data[Index];
        }

        template<uint Index, uint WriteMask = 0xFFFF'FFFF>
        void write(u8 byte)
        {
            static_assert(Type & 0x2);
            static_assert(Index < Size);

            constexpr uint mask = (Mask & WriteMask) >> (CHAR_BIT * Index); 

            data[Index] = byte & mask;
        }

        union
        {
            u8 data[Size];
            T value{Init};
        };
    };
}

template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, typename T = eggcpt::stdint_t<Size, 1>> using RegisterR = detail::RegisterBase<Size, Mask, Init, 0x1, T>;
template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, typename T = eggcpt::stdint_t<Size, 1>> using RegisterW = detail::RegisterBase<Size, Mask, Init, 0x2, T>;
template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, typename T = eggcpt::stdint_t<Size, 1>> using Register  = detail::RegisterBase<Size, Mask, Init, 0x3, T>;
