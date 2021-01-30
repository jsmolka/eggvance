#pragma once

#include "base/register.h"

class Channel : public XRegister<u64>
{
public:
    uint timer   = 0;
    uint sample  = 0;
    uint enabled = 0;
};
