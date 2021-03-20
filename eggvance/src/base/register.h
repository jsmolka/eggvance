#pragma once

#include "bit.h"
#include "int.h"

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
class RegisterBase
{
public:
    static_assert(std::is_integral_v<Integral>);

    RegisterBase(Integral mask = Mask)
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
class RegisterR : public RegisterBase<Integral, Mask>
{
public:
    using RegisterBase<Integral, Mask>::RegisterBase;
    using RegisterBase<Integral, Mask>::read;
};

template<typename Integral, Integral Mask = ones_v<Integral>>
class RegisterW : public RegisterBase<Integral, Mask>
{
public:
    using RegisterBase<Integral, Mask>::RegisterBase;
    using RegisterBase<Integral, Mask>::write;
};

template<typename Integral, Integral Mask = ones_v<Integral>>
class Register : public RegisterBase<Integral, Mask>
{
public:
    using RegisterBase<Integral, Mask>::RegisterBase;
    using RegisterBase<Integral, Mask>::read;
    using RegisterBase<Integral, Mask>::write;
};
