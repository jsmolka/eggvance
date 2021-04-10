#pragma once

#include <glad/glad.h>
#include <shell/predef.h>

#if SHELL_CC_MSVC
#  include <SDL2/SDL_opengl.h>
#else
#  include "SDL.h"
#endif
