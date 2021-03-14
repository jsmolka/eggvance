#pragma once

#include "bit.h"
#include "int.h"

template<typename Integral, uint Mask>
class RegisterBase
{
public:
    static_assert(std::is_integral_v<Integral>);

    static constexpr uint kMask = Mask;
    static constexpr uint kSize = sizeof(Integral);

    union
    {
        u8 data[kSize];
        Integral value = 0;
    };

protected:
    template<uint Index>
    u8 read() const
    {
        static_assert(Index < kSize);

        return data[Index];
    }

    template<uint Index, uint WriteMask = 0xFFFF'FFFF>
    void write(u8 byte)
    {
        static_assert(Index < kSize);

        data[Index] = byte & bit::byte<Index>(Mask & WriteMask);
    }
};

template<typename Integral, uint Mask = 0xFFFF'FFFF>
class RegisterR : public RegisterBase<Integral, Mask>
{
public:
    using RegisterBase<Integral, Mask>::RegisterBase;
    using RegisterBase<Integral, Mask>::read;
};

template<typename Integral, uint Mask = 0xFFFF'FFFF>
class RegisterW : public RegisterBase<Integral, Mask>
{
public:
    using RegisterBase<Integral, Mask>::RegisterBase;
    using RegisterBase<Integral, Mask>::write;
};

template<typename Integral, uint Mask = 0xFFFF'FFFF>
class Register : public RegisterBase<Integral, Mask>
{
public:
    using RegisterBase<Integral, Mask>::RegisterBase;
    using RegisterBase<Integral, Mask>::read;
    using RegisterBase<Integral, Mask>::write;
};

namespace
{

template<typename Integral>
struct ones : std::integral_constant<Integral, static_cast<Integral>(~0ULL)>
{
    static_assert(std::is_integral_v<Integral>);
};

template<typename Integral>
constexpr inline Integral ones_v = ones<Integral>::value;

}  // namespace

template<typename Integral, Integral Mask>
class XRegisterBase
{
public:
    static_assert(std::is_integral_v<Integral>);

    XRegisterBase(Integral mask = Mask)
        : mask(mask) {}

    union
    {
        Integral value = 0;
        u8 bytes[sizeof(Integral)];
    };

    const Integral mask;

protected:
    u8 read(uint index) const
    {
        SHELL_ASSERT(index < sizeof(Integral));

        return bit::byte(value & mask, index);
    }

    bool write(uint index, u8 byte)
    {
        SHELL_ASSERT(index < sizeof(Integral));

        bool changed = byte != bytes[index];
        bytes[index] = byte;

        return changed;
    }
};

template<typename Integral, Integral Mask = ones_v<Integral>>
class XRegisterR : public XRegisterBase<Integral, Mask>
{
public:
    using XRegisterBase<Integral, Mask>::XRegisterBase;
    using XRegisterBase<Integral, Mask>::read;
};

template<typename Integral, Integral Mask = ones_v<Integral>>
class XRegisterW : public XRegisterBase<Integral, Mask>
{
public:
    using XRegisterBase<Integral, Mask>::XRegisterBase;
    using XRegisterBase<Integral, Mask>::write;
};

template<typename Integral, Integral Mask = ones_v<Integral>>
class XRegister : public XRegisterBase<Integral, Mask>
{
public:
    using XRegisterBase<Integral, Mask>::XRegisterBase;
    using XRegisterBase<Integral, Mask>::read;
    using XRegisterBase<Integral, Mask>::write;
};
