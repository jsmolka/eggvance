#pragma once

#include <ctime>

#include "gpio.h"

class Rtc : public Gpio
{
public:
    Rtc();

    void reset() final;

protected:
    u16 readPort() final;
    void writePort(u16 half) final;

private:
    static constexpr uint kParameterBytes[8] = {
        0, 0, 7, 0, 1, 0, 3, 0
    };

    enum Pin
    {
        kPinSck,
        kPinSio,
        kPinCs
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
        Command,
        Receive,
        Transmit
    } state = State::Command;

    struct Port
    {
        uint sck = 0;
        uint sio = 0;
        uint cs  = 0;
    } port;

    struct Control
    {
        uint unknown    = 0;
        uint minute_irq = 0;
        uint format_24h = 0;
    } control;

    uint data = 0;
    uint reg  = 0;
    uint bit  = 0;
    uint byte = 0;
    uint buffer[7] = {};

    void readRegister();
    void writeRegister();

    std::tm readBcdTime() const;
    
    bool readByteSio();
    void receiveCommandSio();
    void receiveBufferSio();
    void transmitBufferSio();
};
