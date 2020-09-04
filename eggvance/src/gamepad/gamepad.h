#pragma once

#include "gamepad/io.h"

class Gamepad
{
public:
    friend class Io;

    void poll();

private:
    KeyInput input;
    KeyControl control;
};

inline Gamepad gamepad;
