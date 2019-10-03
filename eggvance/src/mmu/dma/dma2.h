#pragma once

#include "dma.h"

class DMA2 : public DMA
{
public:
    using DMA::DMA;

    virtual Status emulate(int& cycles) final;
};
