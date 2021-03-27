#include "pipeline.h"

u32 Pipeline::operator[](uint index) const
{
    return data[index];
}

u32& Pipeline::operator[](uint index)
{
    return data[index];
}
