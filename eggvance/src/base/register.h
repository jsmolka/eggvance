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

    template<uint size> struct ValueType;
    template<> struct ValueType<1> { using type =  u8; };
    template<> struct ValueType<2> { using type = u16; };
    template<> struct ValueType<4> { using type = u32; };

    template<uint size, uint mask, uint type>
    class RegisterBase
    {
    public:
        virtual ~RegisterBase() = default;

        template<uint index>
        u8 read() const
        {
            if constexpr (static_cast<bool>(type & kRegisterTypeR))
                return data[index] & byteMask<index>();
            else
                return 0;
        }

        template<uint index>
        void write(u8 byte)
        {
            if constexpr (static_cast<bool>(type & kRegisterTypeW))
                data[index] = byte & byteMask<index>();
        }

        static constexpr uint kSize = size;
        static constexpr uint kMask = mask;
        static constexpr uint kType = type;

        union
        {
            u8 data[size] = { 0 };
            typename ValueType<size>::type value;
        };

    protected:
        template<uint index>
        static constexpr u8 byteMask()
        {
            static_assert(index < size);

            return static_cast<u8>(mask >> (CHAR_BIT * index));
        }
    };

}

template<uint size, uint mask = 0xFFFF'FFFF> using XRegister  = detail::RegisterBase<size, mask, detail::kRegisterTypeR | detail::kRegisterTypeW>;
template<uint size, uint mask = 0xFFFF'FFFF> using XRegisterR = detail::RegisterBase<size, mask, detail::kRegisterTypeR>;
template<uint size, uint mask = 0xFFFF'FFFF> using XRegisterW = detail::RegisterBase<size, mask, detail::kRegisterTypeW>;
