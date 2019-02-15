#pragma once

/**
 * Todo:
 * - split memory into actually used parts
 */

#include <string>
#include <vector>

#include "common/integer.h"
#include "common/internal.h"
#include "registers.h"

class MMU : public Internal
{
public:
    MMU();

    void reset() final;

    bool loadRom(const std::string& file);

    u8 readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    void writeByte(u32 addr, u8 byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    DisplayControl dispcnt;
    DisplayStatus dispstat;
    BackgroundControl bgcnt[4];

private:
    std::vector<u8> memory;
};
