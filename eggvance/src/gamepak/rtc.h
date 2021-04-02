#pragma once

#include "gpio.h"
#include "pin.h"
#include "serialbuffer.h"

class Rtc : public Gpio
{
public:
    Rtc();

    void reset() final;

protected:
    u16 readPort() final;
    void writePort(u16 half) final;

private:
    static constexpr uint kDataBits[8] = { 0, 0, 56, 0, 8, 0, 24, 0 };

    enum class Register
    {
        ForceReset = 0,
        DateTime   = 2,
        ForceIrq   = 3,
        Control    = 4,
        Time       = 6
    };

    enum class State
    {
        InitOne,
        InitTwo,
        Command,
        Receive,
        Transmit,
        Finalize
    } state = State::InitOne;

    struct Control
    {
        uint unknown1   = 0;
        uint minute_irq = 0;
        uint unknown2   = 0;
        uint format_24h = 1;
    } control;

    struct Port
    {
        enum class Bit { Sck, Sio, Cs };

        Pin sck;
        Pin sio;
        Pin cs;
    } port;
    
    void setState(State state);
    void receiveCommandBit();
    void receiveDataBit();
    void transmitDataBit();

    void readRegister();
    void writeRegister();

    uint reg = 0;
    SerialBuffer<u64> data;
    SerialBuffer<u64> buffer;
};
