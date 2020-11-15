#pragma once

#include <ctime>

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
    static constexpr uint kDataBits[8] = {
        0, 0, 56, 0, 8, 0, 24, 0
    };

    enum Bit
    {
        kBitSck,
        kBitSio,
        kBitCs
    };

    enum Register
    {
        kRegForceReset = 0,
        kRegDateTime   = 2,
        kRegForceIrq   = 3,
        kRegControl    = 4,
        kRegTime       = 6
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
        Pin sck;
        Pin sio;
        Pin cs;
    } port;
    
    void setState(State state);
    void receiveCommand();
    void receiveData();
    void transmitData();

    std::tm readBcdTime() const;
    void readRegister();
    void writeRegister();

    uint reg = 0;
    SerialBuffer<u64> data;
    SerialBuffer<u64> buffer;
};
