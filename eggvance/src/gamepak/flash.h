#pragma once

#include "save.h"

class Flash : public Save
{
public:
    Flash(const std::string& file, Save::Type type);

    virtual u8 readByte(u32 addr) final;
    virtual void writeByte(u32 addr, u8 byte) final;

private:
    enum Command
    {
        CMD_ERASE         = 0xAA5580,
        CMD_ERASE_CHIP    = 0xAA5510,
        CMD_ERASE_SECTOR  = 0xAA5530,
        CMD_WRITE_BYTE    = 0xAA55A0,
        CMD_SWITCH_BANK   = 0xAA55B0,
        CMD_CHIP_ID_ENTER = 0xAA5590,
        CMD_CHIP_ID_EXIT  = 0xAA55F0
    };

    bool id;
    bool erase;
    u32 command;
    u8* bank;
};
