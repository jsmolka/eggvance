#pragma once

#include "common/integer.h"

class Register
{
public:
    virtual ~Register() = default;

    virtual void write(int pos, u8 byte) = 0;
};

class DisplayControl : public Register
{
public:
    void write(int pos, u8 byte) final;

    int bg_mode;
    bool gbc_mode;
    int frame_select;
    bool process_hblank;
    int obj_mapping;
    bool forced_blank;
    bool bg0_enable;
    bool bg1_enable;
    bool bg2_enable;
    bool bg3_enable;
    bool oam_enable;
    bool win0_enable;
    bool win1_enable;
    bool obj_win_enable;
};

class DisplayStatus : public Register
{
public:
    void write(int pos, u8 byte) final;

    bool vblank_flag;
    bool hblank_flag;
    bool vcount_flag;
    bool vblank_irq_enable;
    bool hblank_irq_enable;
    bool vcount_irq_enable;
    int vcount_trigger;
};

class BackgroundControl : public Register
{
public:
    void write(int pos, u8 byte) final;

    int priority;
    u32 data_addr;
    bool mosaic;
    int palette;
    u32 map_addr;
    int screen_over;
    int map_size;
};