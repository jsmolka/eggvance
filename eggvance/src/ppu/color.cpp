#include "color.h"

#include <cmath>

#include "base/bit.h"

void Color::init(bool color_correct)
{
    for (u32 color = 0; color < lut.size(); ++color)
    {
        uint r = (bit::seq< 0, 5>(color) << 3) | 0x7;
        uint g = (bit::seq< 5, 5>(color) << 3) | 0x7;
        uint b = (bit::seq<10, 5>(color) << 3) | 0x7;

        if (color_correct)
        {
            constexpr double kDarken   = 0.5;
            constexpr double kGammaOut = 2.2;
            constexpr double kGammaLcd = 2.5;

            double rs = std::pow(r / 255.0, kGammaOut + kDarken);
            double gs = std::pow(g / 255.0, kGammaOut + kDarken);
            double bs = std::pow(b / 255.0, kGammaOut + kDarken);

            double rd = std::pow(0.84 * rs + 0.18 * gs + 0.00 * bs, 1 / kGammaLcd + kDarken * 0.125);
            double gd = std::pow(0.09 * rs + 0.67 * gs + 0.26 * bs, 1 / kGammaLcd + kDarken * 0.125);
            double bd = std::pow(0.15 * rs + 0.10 * gs + 0.75 * bs, 1 / kGammaLcd + kDarken * 0.125);

            r = static_cast<uint>(std::min(1.0, rd) * 255);
            g = static_cast<uint>(std::min(1.0, gd) * 255);
            b = static_cast<uint>(std::min(1.0, bd) * 255);
        }

        lut[color] = 0xFF00'0000 | (r << 16) | (g << 8) | b;
    }
}

uint Color::toArgb(u16 color)
{
    return lut[color];
}
