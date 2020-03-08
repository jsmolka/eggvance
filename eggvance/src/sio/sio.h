#pragma once

#include "io/serial.h"

class SIO
{
public:
    friend class IO;

private:
    SerialIO io;
};

extern SIO sio;
