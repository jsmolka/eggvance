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
inline constexpr Integral ones_v = ones<Integral>::value;

}  // namespace

template<typename Integral, Integral Mask>
class RegisterBase
{
public:
    static_assert(std::is_integral_v<Integral>);

    RegisterBase(Integral mask = Mask)
        : mask(mask) {}

    const Integral mask;

protected:
    u8 read(uint index) const
    {
        return bit::byte(data, index);
    }

    void write(uint index, u8 byte)
    {
        bit::byteRef(raw,  index) = byte;
        bit::byteRef(data, index) = byte;

        data &= mask;
    }

    Integral raw  = 0;
    Integral data = 0;
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
