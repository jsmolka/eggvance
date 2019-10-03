#pragma once

#include "dma.h"

class DMA3 : public DMA
{
public:
    using DMA::DMA;

    virtual Status emulate(int& cycles) final;
};
