#pragma once

#include "register.h"

class TimerData : public Register<u16>
{
public:
    TimerData(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;
};
