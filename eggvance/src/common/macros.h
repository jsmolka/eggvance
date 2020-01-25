#pragma once

#include <cassert>

#define EGG_ASSERT(cond, msg) assert((cond) && msg)

#ifdef NDEBUG
#  ifdef _MSC_VER
#  define EGG_UNREACHABLE __assume(0)
#  else
#  define EGG_UNREACHABLE __builtin_unreachable()
#  endif
#else
#  define EGG_UNREACHABLE EGG_ASSERT(false, "Unreachable")
#endif
