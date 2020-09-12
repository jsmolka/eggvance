#pragma once

#include <eggcpt/env.h>

#if EGGCPT_CC_MSVC || EGGCPT_CC_EMSCRIPTEN
#  include <SDL2/SDL.h>
#else
#  include "SDL.h"
#endif
