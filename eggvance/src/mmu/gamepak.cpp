#include "gamepak.h"

#include <filesystem>
#include <fstream>

#include "common/format.h"

namespace fs = std::filesystem;

GamePak::GamePak(const std::string& file)
{
    fs::path path = file;
    path.replace_extension(".sav");
    save_file = path.string();

    if (valid = read(file, data))
    {
        parseHeader();
        detectSave();

        switch (save_type)
        {
        case Save::SRAM:
            if (fs::exists(save_file))
                read(save_file, sram);
            else
                sram.resize(0x8000, 0);
            break;
        }
    }
}

GamePak::~GamePak()
{
    switch (save_type)
    {
    case Save::SRAM:
        write(save_file, sram);
        break;
    }
}

u8 GamePak::readByte(u32 addr) const
{
    return addr < data.size() ? data[addr] : 0;
}

u8 GamePak::readSaveByte(u32 addr) const
{
    switch (save_type)
    {
    case Save::SRAM:
        return sram[addr];
    }
    return 0;
}

void GamePak::writeSaveByte(u32 addr, u8 byte)
{
    switch (save_type)
    {
    case Save::SRAM:
        sram[addr] = byte;
        break;
    }
}

bool GamePak::read(const std::string& file, std::vector<u8>& dst)
{
    std::ifstream stream(file, std::ios::binary);
    if (!stream.is_open())
        return false;

    stream.seekg(0, std::ios::end);
    std::streampos size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    dst.resize(size);
    stream.read(reinterpret_cast<char*>(dst.data()), size);

    return true;
}

void GamePak::write(const std::string& file, std::vector<u8>& src)
{
    std::ofstream stream(file, std::ofstream::out);
    if (!stream.is_open())
        return;

    stream.write(reinterpret_cast<char*>(src.data()), src.size());
}

std::string GamePak::makeString(u8* data, int size)
{
    return std::string(reinterpret_cast<char*>(data), size);
}

bool GamePak::isSaveType(u8* data, const std::vector<u8>& type)
{
    for (u8 c : type)
    {
        if (c != *data)
            return false;
        data++;
    }
    return true;
}

void GamePak::parseHeader()
{
    header.title = makeString(&data[0xA0], 12);
    header.code  = makeString(&data[0xAC], 4);
    header.maker = makeString(&data[0xB0], 2);
}

void GamePak::detectSave()
{
    static const std::vector<u8> id_sram   = { 'S', 'R', 'A', 'M' };
    static const std::vector<u8> id_flash  = { 'F', 'L', 'A', 'S', 'H' };
    static const std::vector<u8> id_eeprom = { 'E', 'E', 'P', 'R', 'O', 'M' };

    // Some range checking would be nice
    for (int x = 0xC0; x < data.size(); ++x)
    {
        u8* ptr = &data[x];
        if (isSaveType(ptr, id_sram))
        {
            fmt::printf("SRAM");
            save_type = Save::SRAM;
            return;
        }
        if (isSaveType(ptr, id_flash))
        {
            fmt::printf("FLASH");
            save_type = Save::FLASH;
            return;
        }
        if (isSaveType(ptr, id_eeprom))
        {
            fmt::printf("EEPROM");
            save_type = Save::EEPROM;
            return;
        }
    }
    save_type = Save::NONE;
}
