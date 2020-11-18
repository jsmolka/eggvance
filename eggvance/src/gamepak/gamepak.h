#pragma once

#include <memory>
#include <vector>

#include "header.h"
#include "overwrites.h"

class GamePak
{
public:
    uint size() const;

    bool isEepromAccess(u32 addr) const;

    u8  readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    void load(fs::path gba, fs::path sav);

    Header header;
    std::unique_ptr<Gpio> gpio;
    std::unique_ptr<Save> save;

private:
    template<typename Integral>
    Integral read(u32 addr) const;

    template<typename Integral>
    void write(u32 addr, Integral value);

    fs::path file;
    std::vector<u8> rom;
};

inline GamePak gamepak;
