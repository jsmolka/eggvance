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

template<typename Integral, Integral Mask>
class XRegisterBase
{
public:
    static_assert(std::is_integral_v<Integral>);

    XRegisterBase() = default;
    XRegisterBase(Integral mask) : mask(mask) {}

    template<std::size_t Index, std::size_t Size>
    Integral seq() const
    {
        return bit::seq<Index, Size>(data);
    }

    Integral data = 0;
    Integral mask = Mask;

protected:
    u8 read(std::size_t index) const
    {
        SHELL_ASSERT(index < sizeof(Integral));

        u8 data = bit::byte(this->data, index);
        u8 mask = bit::byte(this->mask, index);

        return data & mask;
    }

    void write(std::size_t index, u8 byte)
    {
        SHELL_ASSERT(index < sizeof(Integral));

        reinterpret_cast<u8*>(&data)[index] = byte;
    }
};

template<typename Integral, Integral Mask = -1>
class XRegisterR : public XRegisterBase<Integral, Mask>
{
public:
    using XRegisterBase<Integral, Mask>::XRegisterBase;
    using XRegisterBase<Integral, Mask>::read;
};

template<typename Integral, Integral Mask = -1>
class XRegisterW : public XRegisterBase<Integral, Mask>
{
public:
    using XRegisterBase<Integral, Mask>::XRegisterBase;
    using XRegisterBase<Integral, Mask>::write;
};

template<typename Integral, Integral Mask = -1>
class XRegister : public XRegisterBase<Integral, Mask>
{
public:
    using XRegisterBase<Integral, Mask>::XRegisterBase;
    using XRegisterBase<Integral, Mask>::read;
    using XRegisterBase<Integral, Mask>::write;
};
