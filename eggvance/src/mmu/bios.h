#pragma once

#include <string>

#include "ram.h"

class BIOS
{
public:
    BIOS(const std::string& file);

    void reset();
    void setPC(u32* pc);

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    bool valid;

private:
    enum LastFetched
    {
        LF_BRANCH_ROM = 0x0DC,
        LF_BRANCH_IRQ = 0x134,
        LF_RETURN_IRQ = 0x13C,
        LF_RETURN_SWI = 0x188
    };

    void protect(u32& addr);
    bool read(const std::string& file);
    static u64 hash(u32* data, int size);

    u32* pc;
    u32 last_fetched;
    RAM<0x4000> data;
};
