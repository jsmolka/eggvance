#pragma once

#include "io.h"

class Gamepad
{
public:
    void poll();

    KeyInput input;
    KeyControl control;
};

inline Gamepad gamepad;
