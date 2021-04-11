#pragma once

#include <shell/predef.h>
#include <shell/windows.h>
#include <glad/glad.h>

#if SHELL_CC_MSVC
#  include <SDL2/SDL_opengl.h>
#else
#  include "SDL.h"
#endif
