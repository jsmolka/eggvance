#pragma once

#include <memory>
#include <vector>

#include "header.h"
#include "overwrites.h"

class GamePak
{
public:
    std::size_t size() const;

    u8  readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    bool isEeprom(u32 addr) const;

    void loadRom(const fs::path& file, bool load_save);
    void loadSave(const fs::path& file);

    Header header;
    std::unique_ptr<Gpio> gpio;
    std::unique_ptr<Save> save;

private:
    static u32 readUnused(u32 addr);

    template<typename T>
    T read(u32 addr) const;

    void initGpio(Gpio::Type type);
    void initSave(const fs::path& file, Save::Type type);

    fs::path file;
    std::vector<u8> rom;
};

inline GamePak gamepak;
