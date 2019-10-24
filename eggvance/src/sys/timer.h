#pragma once

#include "regs/tmcnt.h"

class Timer
{
public:
    void reset();

    void run(int cycles);

    template<int index>
    inline u8 readByte();
    template<int index>
    void writeByte(u8 byte);

    int id;
    Timer* next;

private:
    void runInternal(int cycles);
    void updateData();
    void calculate();

    constexpr static int prescalers[4] = { 1, 64, 256, 1024 };

    int counter;
    int overflow;

    TmCnt control;
};

template<int index>
inline u8 Timer::readByte()
{
    switch (index) 
    {
    case 0: updateData(); return control.readByte<0>();
    case 1: updateData(); return control.readByte<1>();
    case 2: return control.readByte<2>();
    case 3: return control.readByte<3>();
    }
    return 0;
}

template<int index>
inline void Timer::writeByte(u8 byte)
{
    updateData();
    switch (index)
    {
    case 0: control.writeByte<0>(byte); break;
    case 1: control.writeByte<1>(byte); break;
    case 2: 
    {
        int enabled = control.enabled;
        control.writeByte<2>(byte);
        if (!enabled && control.enabled)
            control.data = control.initial;
        break;
    }
    case 3: control.writeByte<3>(byte); break;
    }
    calculate();
}
