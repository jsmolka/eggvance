#pragma once

#include "base/integer.h"

namespace detail
{
    template<uint Size> struct StorageTypeImpl;
    template<> struct StorageTypeImpl<1> { using Type =  u8; };
    template<> struct StorageTypeImpl<2> { using Type = u16; };
    template<> struct StorageTypeImpl<4> { using Type = u32; };
}

template<uint Size>
using StorageType = typename detail::StorageTypeImpl<Size>::Type;
