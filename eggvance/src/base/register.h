#pragma once

#include <climits>
#include <type_traits>

#include "base/integer.h"

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
    enum RegisterType
    {
        kRegisterTypeR = 1 << 0,
        kRegisterTypeW = 1 << 1,
    };

    template<uint Size> struct ValueType;
    template<> struct ValueType<1> { using Type =  u8; };
    template<> struct ValueType<2> { using Type = u16; };
    template<> struct ValueType<4> { using Type = u32; };

    template<uint Size, uint Mask, uint Type>
    class RegisterBase
    {
    public:
        static constexpr uint kSize = Size;
        static constexpr uint kMask = Mask;
        static constexpr uint kType = Type;

        virtual ~RegisterBase() = default;

        template<uint Index>
        u8 read() const
        {
            static_assert(Index < Size);

            if constexpr (static_cast<bool>(Type & kRegisterTypeR))
                return data[Index];
            else
                return 0;
        }

        template<uint Index, uint WriteMask = 0xFFFF'FFFF>
        void write(u8 byte)
        {
            static_assert(Index < Size);

            if constexpr (static_cast<bool>(Type & kRegisterTypeW))
                data[Index] = byte & ((Mask & WriteMask) >> (CHAR_BIT * Index));
        }

        union
        {
            u8 data[Size];
            typename ValueType<Size>::Type value{};
        };
    };
}

template<uint Size, uint Mask = 0xFFFF'FFFF> using XRegister  = detail::RegisterBase<Size, Mask, detail::kRegisterTypeR | detail::kRegisterTypeW>;
template<uint Size, uint Mask = 0xFFFF'FFFF> using XRegisterR = detail::RegisterBase<Size, Mask, detail::kRegisterTypeR>;
template<uint Size, uint Mask = 0xFFFF'FFFF> using XRegisterW = detail::RegisterBase<Size, Mask, detail::kRegisterTypeW>;
