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

    int mode;
    bool gbc_mode;
    int bitmap_addr;
    bool process_hblank;
    int sprite_dim;
    bool force_blank;
    bool bg0_enable;
    bool bg1_enable;
    bool bg2_enable;
    bool bg3_enable;
    bool oam_enable;
    bool win0_enable;
    bool win1_enable;
    bool sprite_win_enable;
};

class DisplayStatus : public Register
{
public:
    void write(int pos, u8 byte) final;

    int v_status;
    int h_status;
    bool vcount_triggered;
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
    int data_addr;
    bool mosaic;
    int palette_type;
    int map_addr;
    int screen_over;
    int map_size;
};