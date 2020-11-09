#pragma once

#include <memory>
#include <vector>

#include "header.h"
#include "overwrites.h"

class GamePak
{
public:
    uint size() const;

    u8  readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    bool isEepromAccess(u32 addr) const;

    void loadRom(const fs::path& file, bool save);
    void loadSave(const fs::path& file);

    Header header;
    std::unique_ptr<Gpio> gpio;
    std::unique_ptr<Save> save;

private:
    static u32 readUnused(u32 addr);

    template<typename Integral>
    Integral read(u32 addr) const;
    template<typename Integral>
    void write(u32 addr, Integral value);

    void initGpio(Gpio::Type type);
    void initSave(const fs::path& file, Save::Type type);

    fs::path file;
    std::vector<u8> rom;
};

inline GamePak gamepak;
