#pragma once

#include <cassert>

#ifdef EGG_DEBUG
#  define EGG_ASSERT(cond, msg) assert((cond) && msg)
#else
#  define EGG_ASSERT(cond, msg) static_cast<void>(0)
#endif

#ifdef _MSC_VER
#  ifdef EGG_DEBUG
#    define EGG_UNREACHABLE EGG_ASSERT(false, "Unreachable")
#  else
#    define EGG_UNREACHABLE __assume(0)
#  endif
#else
#  define EGG_UNREACHABLE static_cast<void>(0)
#endif
