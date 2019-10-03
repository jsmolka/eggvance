#pragma once

#include "dma.h"

class DMA1 : public DMA
{
public:
    using DMA::DMA;

    virtual Status emulate(int& cycles) final;
};
