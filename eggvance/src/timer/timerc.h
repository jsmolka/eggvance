#pragma once

#include <array>
#include <functional>
#include <vector>

#include "base/constants.h"
#include "base/macros.h"
#include "timer/timer.h"

class TimerController
{
public:
    TimerController();

    void run(int cycles);
    void runUntilIrq(int& cycles);

    template<uint Addr> u8 read();
    template<uint Addr> void write(u8 byte);

private:
    void runTimers();
    void schedule();
    void reschedule();

    uint count{};
    uint event{};

    std::array<Timer, 4> timers = { 1, 2, 3, 4 };
    std::vector<std::reference_wrapper<Timer>> active;
};

inline TimerController timerc;

template<uint Addr>
u8 TimerController::read()
{
    #define PROLOGUE runTimers()

    INDEXED_IF2(Addr, kRegTimer0Count  , PROLOGUE; return timers[0].io.count.read<kIndex>());
    INDEXED_IF2(Addr, kRegTimer0Control, return timers[0].io.control.read<kIndex>());
    INDEXED_IF2(Addr, kRegTimer1Count  , PROLOGUE; return timers[1].io.count.read<kIndex>());
    INDEXED_IF2(Addr, kRegTimer1Control, return timers[1].io.control.read<kIndex>());
    INDEXED_IF2(Addr, kRegTimer2Count  , PROLOGUE; return timers[2].io.count.read<kIndex>());
    INDEXED_IF2(Addr, kRegTimer2Control, return timers[2].io.control.read<kIndex>());
    INDEXED_IF2(Addr, kRegTimer3Count  , PROLOGUE; return timers[3].io.count.read<kIndex>());
    INDEXED_IF2(Addr, kRegTimer3Control, return timers[3].io.control.read<kIndex>());

    EGGCPT_UNREACHABLE;
    return 0;

    #undef PROLOGUE
}

template<uint Addr>
void TimerController::write(u8 byte)
{
    #define PROLOGUE(timer)                     \
        runTimers();                            \
        uint enable = timer.io.control.enable

    #define EPILOGUE(timer)                     \
        if (!enable && timer.io.control.enable) \
            timer.init();                       \
        else if (enable)                        \
            timer.update();                     \
        schedule()

    INDEXED_IF2(Addr, kRegTimer0Count  , timers[0].io.count.write<kIndex>(byte));
    INDEXED_IF1(Addr, kRegTimer0Control, PROLOGUE(timers[0]); timers[0].io.control.write<kIndex>(byte); EPILOGUE(timers[0]));
    INDEXED_IF2(Addr, kRegTimer1Count  , timers[1].io.count.write<kIndex>(byte));
    INDEXED_IF1(Addr, kRegTimer1Control, PROLOGUE(timers[1]); timers[1].io.control.write<kIndex>(byte); EPILOGUE(timers[1]));
    INDEXED_IF2(Addr, kRegTimer2Count  , timers[2].io.count.write<kIndex>(byte));
    INDEXED_IF1(Addr, kRegTimer2Control, PROLOGUE(timers[2]); timers[2].io.control.write<kIndex>(byte); EPILOGUE(timers[2]));
    INDEXED_IF2(Addr, kRegTimer3Count  , timers[3].io.count.write<kIndex>(byte));
    INDEXED_IF1(Addr, kRegTimer3Control, PROLOGUE(timers[3]); timers[3].io.control.write<kIndex>(byte); EPILOGUE(timers[3]));

    #undef PROLOGUE
    #undef EPILOGUE
}
