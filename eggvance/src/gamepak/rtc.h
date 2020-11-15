#pragma once

#include <ctime>

#include "gpio.h"
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
        InitOne,
        InitTwo,
        Command,
        Receive,
        Transmit,
        Finalize
    } state = State::InitOne;

    struct Port
    {
        bool isBitTransfer(const Port& prev);

        uint sck = 0;
        uint sio = 0;
        uint cs  = 0;
    } port;

    struct Control
    {
        uint unknown    = 0;
        uint minute_irq = 0;
        uint format_24h = 1;
    } control;

    
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
