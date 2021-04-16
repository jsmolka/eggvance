#pragma once

#include <string>
#include <shell/predef.h>
#include <shell/format.h>

#if SHELL_CC_MSVC
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_syswm.h>
#else
#  include "SDL.h"
#endif

