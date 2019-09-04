#pragma once

#include "common/integer.h"

class MicroTimer
{
public:
    MicroTimer();
    ~MicroTimer();

    u64 now() const;
    void sleep(u32 us) const;

private:
    void* timer;
};
