#include "rtc.h"

#include <ctime>
#include <shell/utility.h>

#include "arm/arm.h"
#include "base/bit.h"

Rtc::Rtc()
    : Gpio(Type::Rtc)
{

}

void Rtc::reset()
{
    shell::reconstruct(*this);
}

u16 Rtc::readPort()
{
    return state == State::Transmit
        ? port.sio << 1
        : 0;
}

void Rtc::writePort(u16 half)
{
    if (isGbaToGpio(uint(Port::Bit::Sck))) port.sck = bit::seq<uint(Port::Bit::Sck), 1>(half);
    if (isGbaToGpio(uint(Port::Bit::Sio))) port.sio = bit::seq<uint(Port::Bit::Sio), 1>(half);
    if (isGbaToGpio(uint(Port::Bit::Cs)))  port.cs  = bit::seq<uint(Port::Bit::Cs),  1>(half);

    switch (state)
    {
    case State::InitOne:
        if (port.cs.low())
            setState(State::InitTwo);
        break;

    case State::InitTwo:
        if (port.cs.rising())
            setState(State::Command);
        break;

    case State::Command:
        if (port.cs.low())
            setState(State::InitOne);
        else if (port.sck.rising())
            receiveCommandBit();
        break;

    case State::Receive:
        if (port.cs.low())
            setState(State::InitOne);
        else if (port.sck.rising())
            receiveDataBit();
        break;

    case State::Transmit:
        if (port.cs.low())
            setState(State::InitOne);
        else if (port.sck.rising())
            transmitDataBit();
        break;

    case State::Finalize:
        if (port.cs.falling())
            setState(State::InitOne);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

void Rtc::setState(State state)
{
    this->state = state;
    this->buffer.clear();
}

void Rtc::receiveCommandBit()
{
    buffer.pushl(port.sio);

    if (buffer.size < 8)
        return;

    constexpr auto kFixedBits = 0b0110;

    if (bit::seq<0, 4>(buffer.data) != kFixedBits &&
        bit::seq<4, 4>(buffer.data) == kFixedBits)
    {
        buffer = (buffer & 0xF0) >> 4 | (buffer & 0x0F) << 4;
        buffer = (buffer & 0xCC) >> 2 | (buffer & 0x33) << 2;
        buffer = (buffer & 0xAA) >> 1 | (buffer & 0x55) << 1;
    }

    if (bit::seq<0, 4>(buffer.data) != kFixedBits)
        return;

    reg = bit::seq<4, 3>(buffer.data);

    if (buffer & 0x80)
    {
        readRegister();

        setState(kDataBits[reg] > 0
            ? State::Transmit
            : State::Finalize);
    }
    else
    {
        if (kDataBits[reg] > 0)
        {
            setState(State::Receive);
        }
        else
        {
            writeRegister();

            setState(State::Finalize);
        }
    }
}

void Rtc::receiveDataBit()
{
    buffer.pushl(port.sio);

    if (buffer.size == kDataBits[reg])
    {
        writeRegister();

        setState(State::Finalize);
    }
}

void Rtc::transmitDataBit()
{
    port.sio = data.popr();

    if (data.size == 0)
        setState(State::Finalize);
}

void Rtc::readRegister()
{
    auto bcd = [](auto value)
    {
        return ((value / 10) << 4) | (value % 10);
    };

    auto stamp = std::time(NULL);
    auto time = *std::localtime(&stamp);

    switch (Register(reg))
    {
    case Register::Control:
        data[0] = 0;
        data[0] |= control.unknown1   << 1;
        data[0] |= control.minute_irq << 3;
        data[0] |= control.unknown2   << 5;
        data[0] |= control.format_24h << 6;
        data.size = 8;
        break;

    case Register::DateTime:
        data[0] = bcd(time.tm_year - 100);
        data[1] = bcd(time.tm_mon + 1);
        data[2] = bcd(time.tm_mday);
        data[3] = bcd(time.tm_wday);
        data[4] = bcd(time.tm_hour % (control.format_24h ? 24 : 12));
        data[5] = bcd(time.tm_min);
        data[6] = bcd(time.tm_sec);
        data.size = 56;
        break;

    case Register::Time:
        data[0] = bcd(time.tm_hour % (control.format_24h ? 24 : 12));
        data[1] = bcd(time.tm_min);
        data[2] = bcd(time.tm_sec);
        data.size = 24;
        break;
    }
}

void Rtc::writeRegister()
{
    switch (Register(reg))
    {
    case Register::Control:
        control.unknown1   = bit::seq<1, 1>(buffer.data);
        control.minute_irq = bit::seq<3, 1>(buffer.data);
        control.unknown2   = bit::seq<5, 1>(buffer.data);
        control.format_24h = bit::seq<6, 1>(buffer.data);
        break;

    case Register::ForceReset:
        control = Control();
        break;

    case Register::ForceIrq:
        arm.raise(Irq::GamePak);
        break;
    }
}
