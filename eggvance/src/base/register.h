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

template<typename Integral, Integral kMask>
class RegisterBase
{
public:
    static_assert(std::is_integral_v<Integral>);

    RegisterBase(Integral mask = kMask)
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

template<typename Integral, Integral kMask = ones_v<Integral>>
class RegisterR : public RegisterBase<Integral, kMask>
{
public:
    using RegisterBase<Integral, kMask>::RegisterBase;
    using RegisterBase<Integral, kMask>::read;
};

template<typename Integral, Integral kMask = ones_v<Integral>>
class RegisterW : public RegisterBase<Integral, kMask>
{
public:
    using RegisterBase<Integral, kMask>::RegisterBase;
    using RegisterBase<Integral, kMask>::write;
};

template<typename Integral, Integral kMask = ones_v<Integral>>
class Register : public RegisterBase<Integral, kMask>
{
public:
    using RegisterBase<Integral, kMask>::RegisterBase;
    using RegisterBase<Integral, kMask>::read;
    using RegisterBase<Integral, kMask>::write;
};
