#include "rtc.h"

#include "arm/arm.h"
#include "arm/constants.h"
#include "base/bit.h"

#include <shell/utility.h>

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
    if (state == State::Transmit)
        return port.sio << kPinSio;

    return 1;
}

void Rtc::writePort(u16 half)
{
    Port prev = port;

    if (isGbaToGpio(kPinCs))  port.cs  = bit::seq<kPinCs,  1>(half);
    if (isGbaToGpio(kPinSio)) port.sio = bit::seq<kPinSio, 1>(half);
    if (isGbaToGpio(kPinSck)) port.sck = bit::seq<kPinSck, 1>(half);

    if (!prev.cs && port.cs)
    {
        state = State::Command;
        bit  = 0;
        byte = 0;
    }

    if (!port.cs || !(!prev.sck && port.sck))
        return;

    switch (state)
    {
    case State::Command:
        receiveCommandSio();
        break;

    case State::Receive:
        receiveBufferSio();
        break;

    case State::Transmit:
        transmitBufferSio();
        break;
    }
}

std::tm Rtc::readBcdTime() const
{
    static constexpr uint kDaysSinceMonday[7] = {
        6, 5, 4, 3, 2, 1, 0
    };

    auto toBcd = [](u8 decimal) {
        return ((decimal / 10) << 4) | (decimal % 10);
    };

    auto time = std::time(NULL);
    auto bcd = *std::localtime(&time);

    bcd.tm_year = toBcd(bcd.tm_year - 100);
    bcd.tm_mon  = toBcd(bcd.tm_mon + 1);
    bcd.tm_mday = toBcd(bcd.tm_mday);
    bcd.tm_wday = toBcd(kDaysSinceMonday[bcd.tm_wday]);
    bcd.tm_hour = toBcd(bcd.tm_hour % (control.format_24h ? 24 : 12));
    bcd.tm_min  = toBcd(bcd.tm_min);
    bcd.tm_sec  = toBcd(bcd.tm_sec);

    return bcd;
}

void Rtc::readRegister()
{
    auto time = readBcdTime();

    switch (reg)
    {
    case kRegControl:
        buffer[0] = 0;
        buffer[0] |= control.unknown    << 1;
        buffer[0] |= control.minute_irq << 3;
        buffer[0] |= control.format_24h << 6;
        break;

    case kRegDateTime:
        buffer[0] = time.tm_year;
        buffer[1] = time.tm_mon;
        buffer[2] = time.tm_mday;
        buffer[3] = time.tm_wday;
        buffer[4] = time.tm_hour;
        buffer[5] = time.tm_min;
        buffer[6] = time.tm_sec;
        break;

    case kRegTime:
        buffer[0] = time.tm_hour;
        buffer[1] = time.tm_min;
        buffer[2] = time.tm_sec;
        break;
    }
}

void Rtc::writeRegister()
{
    switch (reg)
    {
    case kRegControl:
        control.unknown    = bit::seq<1, 1>(buffer[0]);
        control.minute_irq = bit::seq<3, 1>(buffer[0]);
        control.format_24h = bit::seq<6, 1>(buffer[0]);
        break;

    case kRegForceReset:
        control = {};
        break;

    case kRegForceIrq:
        arm.raise(kIrqGamePak);
        break;
    }
}

bool Rtc::readByteSio()
{
    data &= ~(1 << bit);
    data |= port.sio << bit;

    if (++bit == 8)
        bit = 0;

    return bit == 0;
}

void Rtc::receiveCommandSio()
{
    if (!readByteSio())
        return;

    if ((data >> 4) == 6)
    {
        data = (data & 0xF0) >> 4 | (data & 0x0F) << 4;
        data = (data & 0xCC) >> 2 | (data & 0x33) << 2;
        data = (data & 0xAA) >> 1 | (data & 0x55) << 1;
    }

    reg  = bit::seq<4, 3>(data);
    bit  = 0;
    byte = 0;

    if (data & 0x80)
    {
        readRegister();

        state = kParameterBytes[reg] > 0
            ? State::Transmit
            : State::Command;
    }
    else
    {
        if (kParameterBytes[reg] > 0)
        {
            state = State::Receive;
        }
        else
        {
            writeRegister();
            state = State::Command;
        }
    }
}

void Rtc::receiveBufferSio()
{
    if (byte < kParameterBytes[reg] && readByteSio())
    {
        buffer[byte] = data;

        if (++byte == kParameterBytes[reg])
        {
            writeRegister();

            state = State::Command;
        }
    }
}

void Rtc::transmitBufferSio()
{
    port.sio = buffer[byte] & 0x1;
    buffer[byte] >>= 1;

    if (++bit == 8)
    {
        bit = 0;
        if (++byte == kParameterBytes[reg])
            state = State::Command;
    }
}
