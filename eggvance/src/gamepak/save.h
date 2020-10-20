#pragma once

#include <vector>

#include "base/int.h"
#include "base/filesystem.h"

class Save
{
public:
    enum class Type
    {
        None,
        Sram,
        Eeprom,
        Flash512,
        Flash1024
    };

    Save();
    explicit Save(Type type);
    virtual ~Save();

    static Type parse(const std::vector<u8>& rom);

    bool init(const fs::path& file);

    virtual void reset();

    virtual u8 read(u32 addr);
    virtual void write(u32 addr, u8 byte);

    const Type type;
    std::vector<u8> data;

protected:
    virtual bool isValidSize() const;

    bool changed = false;

private:
    fs::path file;
};
