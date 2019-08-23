#pragma once

#ifdef _MSC_VER
#  ifdef EGG_DEBUG
#    include <cassert>
#    define UNREACHABLE assert(!"Unreachable line")
#  else
#    define UNREACHABLE __assume(0)
#  endif
#else
#  define UNREACHABLE static_cast<void>(0)
#endif
