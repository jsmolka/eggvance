#pragma once

#ifdef _WIN32
#  ifdef EGG_DEBUG
#    include <cassert>
#    define UNREACHABLE assert(!"Unreachable line")
#  else
#    define UNREACHABLE __assume(0)
#  endif
#else
#  define UNREACHABLE ((void)0)
#endif
