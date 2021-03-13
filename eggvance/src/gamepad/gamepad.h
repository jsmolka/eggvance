#pragma once

#include "io.h"

class Gamepad
{
public:
    void update();
    void tryRaise();

    KeyInput input;
    KeyControl control;
};

inline Gamepad gamepad;
