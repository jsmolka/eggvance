#pragma once

#include "regs/timerdata.h"
#include "regs/timercontrol.h"

class Timer
{
public:
    Timer(int id);

    void reset();

    void run(u64 accumulated);
    
    void start();
    void update();

    bool canChange() const;
    bool canCauseInterrupt() const;

    u64 interruptsAfter() const;

    int id;
    Timer* prev;
    Timer* next;

    TimerData data;
    TimerControl control;

private:
    bool inActiveCascadeChain() const;

    u64 cyclesRemaining() const;

    u64 cycles;
    u64 cycles_max;
    u64 cycles_inital;
    u64 cycles_overflow;
};
