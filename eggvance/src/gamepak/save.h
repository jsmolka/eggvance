#pragma once

#include "rom.h"
#include "base/filesystem.h"
#include "base/int.h"

class Save
{
public:
    enum class Type
    {
        Detect,
        None,
        Sram,
        Eeprom,
        Flash512,
        Flash1024
    };

    Save();
    Save(Type type);
    virtual ~Save();

    static Type parse(const Rom& rom);

    bool load(const fs::path& file);

    virtual void reset();
    virtual u8 read(u32 addr);
    virtual void write(u32 addr, u8 byte);

    const Type type;

protected:
    void resize(std::size_t size);

    virtual bool isValidSize(uint size) const;

    bool changed = false;
    std::vector<u8> data;

private:
    fs::path file;
};
