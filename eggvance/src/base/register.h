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
