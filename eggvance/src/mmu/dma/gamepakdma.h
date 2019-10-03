#pragma once

#include "dma.h"

class GamePakDMA : public DMA
{
public:
    using DMA::DMA;

    virtual bool emulate(int& cycles) final;

private:
    bool readEEPROM(int& cycles);
    bool writeEEPROM(int& cycles);
};
