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
    enum RegisterType
    {
        kRegisterTypeR = 1 << 0,
        kRegisterTypeW = 1 << 1,
    };

    template<uint Size, uint Mask, uint Init, uint Type, typename T>
    struct RegisterBase
    {
        static_assert(Size == sizeof(T), "RegisterBase<uint, uint, uint, T>: Invalid T");

        using ValueType = T;

        static constexpr uint kSize = Size;
        static constexpr uint kMask = Mask;
        static constexpr uint kInit = Init;
        static constexpr uint kType = Type;

        virtual ~RegisterBase() = default;

        //RegisterBase& operator=(ValueType value)
        //{
        //    this->value = value;
        //}

        //operator ValueType() const
        //{
        //    return value;
        //}

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
            ValueType value{ Init };
        };
    };
}

template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, typename T = StorageType<Size>> using XRegister  = detail::RegisterBase<Size, Mask, Init, detail::kRegisterTypeR | detail::kRegisterTypeW, typename T>;
template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, typename T = StorageType<Size>> using XRegisterR = detail::RegisterBase<Size, Mask, Init, detail::kRegisterTypeR, typename T>;
template<uint Size, uint Mask = 0xFFFF'FFFF, uint Init = 0, typename T = StorageType<Size>> using XRegisterW = detail::RegisterBase<Size, Mask, Init, detail::kRegisterTypeW, typename T>;
