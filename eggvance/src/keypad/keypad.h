#pragma once

#include "keypad/io.h"

class Keypad
{
public:
    void update();

    KeypadIo io;
};

inline Keypad keypad;
