#pragma once

#include <algorithm>

#include "base/bit.h"
#include "base/config.h"
#include "base/constants.h"
#include "base/register.h"
#include "ppu/dimensions.h"
#include "ppu/layer.h"

struct PpuIo
{
    class DisplayControl : public Register<2, 0xFFF7>
    {
    public:
        template<uint Index>
        void write(u8 byte)
        {
            Register<kSize, kMask>::write<Index>(byte);

            if (Index == 0)
            {
                mode     = bit::seq<0, 3>(data[Index]);
                frame    = bit::seq<4, 1>(data[Index]) * 0xA000;
                oam_free = bit::seq<5, 1>(data[Index]);
                mapping  = bit::seq<6, 1>(data[Index]);
                blank    = bit::seq<7, 1>(data[Index]);
            }
            else
            {
                layers = bit::seq<0, 5>(data[Index]);
                win0   = bit::seq<5, 1>(data[Index]);
                win1   = bit::seq<6, 1>(data[Index]);
                winobj = bit::seq<7, 1>(data[Index]);
            }
        }

        bool isActive() const
        {
            switch (mode)
            {
            case 0: return 0b11111 & layers;
            case 1: return 0b10111 & layers;
            case 2: return 0b11100 & layers;
            case 3: return 0b10100 & layers;
            case 4: return 0b10100 & layers;
            case 5: return 0b10100 & layers;

            default:
                return false;
            }
        }

        bool isBitmap() const
        {
            return mode > 2;
        }

        uint mode{};
        uint frame{};
        uint oam_free{};
        uint mapping{};
        uint blank{ config.bios_skip };
        uint layers{};
        uint win0{};
        uint win1{};
        uint winobj{};
    }
    dispcnt;

    Register<2> greenswap;

    class DisplayStatus : public Register<2, 0xFF38>
    {
    public:
        template<uint Index>
        u8 read() const
        {
            u8 value = Register<kSize, kMask>::read<Index>();

            if (Index == 0) value |= (vblank << 0) | (hblank << 1) | (vmatch << 2);

            return value;
        }

        template<uint Index>
        void write(u8 byte)
        {
            Register<kSize, kMask>::write<Index>(byte);

            if (Index == 0)
            {
                vblank_irq = bit::seq<3, 1>(data[Index]);
                hblank_irq = bit::seq<4, 1>(data[Index]);
                vmatch_irq = bit::seq<5, 1>(data[Index]);
            }
            else
            {
                vcompare = data[Index];
            }
        }

        uint vblank{};
        uint hblank{};
        uint vmatch{};
        uint vblank_irq{};
        uint hblank_irq{};
        uint vmatch_irq{};
        uint vcompare{};
    }
    dispstat;

    struct VCount : RegisterR<2>
    {
        void next()
        {
            value = (value + 1) % 228;
        }
    }
    vcount;

    struct BGControl : Register<2>
    {
        template<uint Index, uint Mask>
        void write(u8 byte)
        {
            Register<kSize, kMask>::write<Index, Mask>(byte);

            if (Index == 0)
            {
                priority   = bit::seq<0, 2>(data[Index]);
                tile_block = bit::seq<2, 2>(data[Index]) * 0x4000;
                mosaic     = bit::seq<6, 1>(data[Index]);
                color_mode = bit::seq<7, 1>(data[Index]);
            }
            else
            {
                map_block  = bit::seq<0, 5>(data[Index]) * 0x0800;
                wraparound = bit::seq<5, 1>(data[Index]);
                dimensions = bit::seq<6, 2>(data[Index]);
            }
        }

        Dimensions dimsReg() const 
        {
            return Dimensions(
                256 << bit::seq<0, 1>(dimensions),
                256 << bit::seq<1, 1>(dimensions)
            );
        }

        Dimensions dimsAff() const
        {
            return Dimensions(
                128 << dimensions,
                128 << dimensions
            );
        }

        uint priority{};
        uint tile_block{};
        uint mosaic{};
        uint color_mode{};
        uint map_block{};
        uint wraparound{};
        uint dimensions{};
    }
    bgcnt[4];

    RegisterW<2, 0x01FF> bghofs[4];
    RegisterW<2, 0x01FF> bgvofs[4];
    RegisterW<2, 0xFFFF, 0x0100, s16> bgpa[2];
    RegisterW<2, 0xFFFF, 0x0000, s16> bgpb[2];
    RegisterW<2, 0xFFFF, 0x0000, s16> bgpc[2];
    RegisterW<2, 0xFFFF, 0x0100, s16> bgpd[2];

    struct BGReference : RegisterW<4, 0x0FFF'FFFF>
    {
        operator s32() const
        {
            return current;
        }

        template<uint Index>
        void write(u8 byte)
        {
            RegisterW<kSize, kMask>::write<Index>(byte);

            current = bit::signEx<28>(value);
        }

        void hblank(s16 value)
        {
            current += value;
        }

        void vblank()
        {
            current = bit::signEx<28>(value);
        }

        s32 current{};
    };

    BGReference bgx[2];
    BGReference bgy[2];

    struct Window
    {
        void write(u8 byte)
        {
            flags = bit::seq<0, 5>(byte) | LF_BDP;
            blend = bit::seq<5, 1>(byte);
        }

        uint flags{};
        uint blend{};
    };

    struct WindowInside : Register<2, 0x3F3F>
    {
        template<uint Index>
        void write(u8 byte)
        {
            Register<kSize, kMask>::write<Index>(byte);

            if (Index == 0) win0.write(data[Index]);
            if (Index == 1) win1.write(data[Index]);
        }

        Window win0;
        Window win1;
    }
    winin;

    struct WindowOutside : Register<2, 0x3F3F>
    {
        template<uint Index>
        void write(u8 byte)
        {
            Register<kSize, kMask>::write<Index>(byte);

            if (Index == 0) winout.write(data[Index]);
            if (Index == 1) winobj.write(data[Index]);
        }

        Window winout;
        Window winobj;
    }
    winout;

    template<uint Max>
    struct WindowRange : RegisterW<2>
    {
        static constexpr uint kMax = Max;

        template<uint Index>
        void write(u8 byte)
        {
            RegisterW<kSize>::write<Index>(byte);

            max = data[0];
            min = data[1];

            if (max > Max || max < min)
                max = Max;
        }

        bool contains(uint value) const
        {
            return value >= min && value < max;
        }

        uint min{};
        uint max{};
    };

    WindowRange<kScreenW> winh[2];
    WindowRange<kScreenH> winv[2];

    struct Mosaic
    {
        struct Block
        {
            void write(u8 byte)
            {
                x = bit::seq<0, 4>(byte) + 1;
                y = bit::seq<4, 4>(byte) + 1;
            }

            uint mosaicX(uint value) const { return x * (value / x); }
            uint mosaicY(uint value) const { return y * (value / y); }

            uint x{ 1 };
            uint y{ 1 };
        };

        template<uint Index>
        void write(u8 byte)
        {
            static_assert(Index < 2);

            if (Index == 0) bgs.write(byte);
            if (Index == 1) obj.write(byte);
        }

        Block bgs;
        Block obj;
    }
    mosaic;

    struct BlendControl : Register<2, 0x3FFF>
    {
        enum Mode
        {
            kModeDisabled,
            kModeAlpha,
            kModeWhite,
            kModeBlack
        };

        template<uint Index>
        void write(u8 byte)
        {
            Register<kSize, kMask>::write<Index>(byte);

            if (Index == 0)
            {
                upper = bit::seq<0, 6>(data[Index]);
                mode  = bit::seq<6, 2>(data[Index]);
            }
            else
            {
                lower = bit::seq<0, 6>(data[Index]);
            }
        }

        uint mode{};
        uint upper{};
        uint lower{};
    }
    bldcnt;

    struct BlendAlpha : Register<2, 0x1F1F>
    {
        static constexpr uint kMaskR = 0x1F <<  0;
        static constexpr uint kMaskG = 0x1F <<  5;
        static constexpr uint kMaskB = 0x1F << 10;

        template<uint Index>
        void write(u8 byte)
        {
            Register<kSize, kMask>::write<Index>(byte);

            coeff[Index] = std::min<uint>(16u, bit::seq<0, 5>(data[Index]));
        }

        u16 blendAlpha(u16 a, u16 b) const
        {
            uint cr = std::min(kMaskR, ((a & kMaskR) * coeff[0] + (b & kMaskR) * coeff[1]) >> 4);
            uint cg = std::min(kMaskG, ((a & kMaskG) * coeff[0] + (b & kMaskG) * coeff[1]) >> 4);
            uint cb = std::min(kMaskB, ((a & kMaskB) * coeff[0] + (b & kMaskB) * coeff[1]) >> 4);

            return (cr & kMaskR) | (cg & kMaskG) | (cb & kMaskB);
        }

        uint coeff[2]{};
    }
    bldalpha;

    struct BlendFade : RegisterW<2, 0x001F>
    {
        static constexpr uint kMaskR = 0x1F <<  0;
        static constexpr uint kMaskG = 0x1F <<  5;
        static constexpr uint kMaskB = 0x1F << 10;

        template<uint Index>
        void write(u8 byte)
        {
            RegisterW<kSize, kMask>::write<Index>(byte);

            coeff = std::min<uint>(16u, bit::seq<0, 5>(data[0]));
        }

        u16 blendWhite(u16 a) const
        {
            uint r = a & kMaskR;
            uint g = a & kMaskG;
            uint b = a & kMaskB;

            r += ((kMaskR - r) * coeff) >> 4;
            g += ((kMaskG - g) * coeff) >> 4;
            b += ((kMaskB - b) * coeff) >> 4;

            return (r & kMaskR) | (g & kMaskG) | (b & kMaskB);
        }

        u16 blendBlack(u16 a) const
        {
            uint r = a & kMaskR;
            uint g = a & kMaskG;
            uint b = a & kMaskB;

            r -= (r * coeff) >> 4;
            g -= (g * coeff) >> 4;
            b -= (b * coeff) >> 4;

            return (r & kMaskR) | (g & kMaskG) | (b & kMaskB);
        }

        uint coeff{};
    }
    bldfade;
};