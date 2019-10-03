#pragma once

#include "common/integer.h"
#include "mmu/registers/dmaaddress.h"
#include "mmu/registers/dmacontrol.h"

class MMU;

class DMA
{
public:
    enum Timing
    {
        IMMEDIATE = 0,
        VBLANK    = 1,
        HBLANK    = 2,
        REFRESH   = 3
    };

    DMA(int id, MMU& mmu);
    virtual ~DMA() = default;

    bool canStart(Timing timing) const;

    void reset();
    void start();

    virtual bool emulate(int& cycles);

    int id;

protected:
    bool transfer(int& cycles);
    
    void finish();

    MMU& mmu;

    DMAAddress& sad;
    DMAAddress& dad;
    DMAControl& control;

    struct Registers
    {
        u32 sad;
        u32 dad;
    } regs;

    int count;

    int sad_delta;
    int dad_delta;
    int sequential;

    bool running;

private:
    enum AddressControl
    {
        INCREMENT = 0,
        DECREMENT = 1,
        FIXED     = 2,
        RELOAD    = 3
    };

    int addressDelta(AddressControl control);
};
