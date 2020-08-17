#pragma once

// Temporary until eggcpt is public

#include <fmt/format.h>

#include "base/integer.h"
#include "base/bits.h"

namespace bit = bits;

#define EGGCPT_CC_MSVC _MSC_VER
#define EGGCPT_CC_EMSCRIPTEN __EMSCRIPTEN__
