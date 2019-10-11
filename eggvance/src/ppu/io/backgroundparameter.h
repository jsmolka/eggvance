#pragma once

#include "common/integer.h"

struct BackgroundParameter
{
    void reset();

    void writeByte(int index, u8 byte);

    s16 parameter;  // Affine parameter
};
