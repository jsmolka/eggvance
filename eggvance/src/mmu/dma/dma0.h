#pragma once

#include "dma.h"

class DMA0 : public DMA
{
public:
    using DMA::DMA;

    virtual Status emulate(int& cycles) final;
};
