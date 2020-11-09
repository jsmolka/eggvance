#pragma once

#include <shell/env.h>
#include <shell/log/none.h>

#ifndef NO_CONSOLE
#  if SHELL_DEBUG
#    include <shell/log/all.h>
#  else
#    include <shell/log/error.h>
#    include <shell/log/fatal.h>
#  endif
#endif
