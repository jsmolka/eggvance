#pragma once

#include <memory>
#include <vector>

#include "gamepak/header.h"
#include "gamepak/override.h"

class GamePak
{
public:
    std::size_t size() const;

    u8  readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    void load(const fs::path& rom_file, const fs::path& save_file);
    void load(const fs::path& rom_file);
    void loadSave(const fs::path& save_file);

    Header header;
    std::unique_ptr<Gpio> gpio;
    std::unique_ptr<Save> save;

private:
    static u32 readUnused(u32 addr);

    void initGpio(Gpio::Type type);
    void initSave(const fs::path& file, Save::Type type);

    template<typename T>
    T read(u32 addr) const;

    fs::path file;
    std::vector<u8> rom;
};
