#pragma once

#include <shell/predef.h>

#if SHELL_CC_MSVC || SHELL_CC_EMSCRIPTEN
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_syswm.h>
#else
#  include "SDL.h"
#endif
