#pragma once

#include "common/bitfield.h"
#include "common/integer.h"

// Todo: make this a separate class
template<typename T>
using Register = BitField<T, 0, 8 * sizeof(T)>;
