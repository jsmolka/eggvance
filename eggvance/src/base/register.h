#pragma once

#include <climits>

#include "base/integer.h"
#include "base/types.h"

template<uint size>
class RegisterRW
{
public:
    virtual ~RegisterRW() = default;

    template<uint index>
    inline u8 read() const
    {
        static_assert(index < size);

        return data[index];
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < size);

        data[index] = byte;
    }

protected:
    u8 data[size] = { 0 };
};

template<uint size>
class RegisterR : public RegisterRW<size>
{
public:
    template<uint index>
    inline void write(u8) = delete;
};

template<uint size>
class RegisterW : public RegisterRW<size>
{
public:
    template<uint index>
    inline u8 read() const = delete;
};

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
            static_assert(Type & 0b01);
            static_assert(Index < Size);

            return data[Index];
        }

        template<uint Index, uint WriteMask = 0xFFFF'FFFF>
        void write(u8 byte)
        {
            static_assert(Type & 0b10);
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

template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, typename T = StorageType<Size>> using XRegister  = detail::RegisterBase<Size, Mask, Init, 0b11, T>;
template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, typename T = StorageType<Size>> using XRegisterR = detail::RegisterBase<Size, Mask, Init, 0b01, T>;
template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, typename T = StorageType<Size>> using XRegisterW = detail::RegisterBase<Size, Mask, Init, 0b10, T>;
