#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "gamepak/gpio.h"
#include "gamepak/header.h"
#include "gamepak/save.h"

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
    struct Override
    {
        std::string code;
        Save::Type save;
        Gpio::Type gpio;
        bool mirror;
    };

    static u32 readUnused(u32 addr);

    void initSave(const fs::path& file, Save::Type type);

    const static std::vector<Override> overrides;

    fs::path file;
    std::vector<u8> rom;
};
