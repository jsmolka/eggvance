#pragma once

// Temporary until eggcpt is public

#include <cassert>
#include <fmt/format.h>

#include "base/integer.h"
#include "base/bits.h"

namespace bit = bits;

#define EGGCPT_CC_MSVC _MSC_VER
#define EGGCPT_CC_EMSCRIPTEN __EMSCRIPTEN__

#define EGGCPT_ASSERT(condition, ...) assert((condition) && (__VA_ARGS__""))
