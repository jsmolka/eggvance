#pragma once

#include "io/keypadio.h"

class Keypad
{
public:
    friend class Io;

    void process();

private:
    KeypadIO io;
};

extern Keypad keypad;
