#pragma once

#include "dma.h"

class SoundDMA : public DMA
{
public:
    using DMA::DMA;

    virtual bool emulate(int& cycles) final;
};
