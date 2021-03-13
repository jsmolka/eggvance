#pragma once

#include "io.h"

class Keypad
{
public:
    void update();
    void checkInterrupt();

    KeyInput input;
    KeyControl control;
};

inline Keypad keypad;
