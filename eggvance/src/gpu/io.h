#pragma once

#include <algorithm>

#include "layer.h"
#include "point.h"
#include "base/config.h"
#include "base/register.h"

struct PpuIo
{
    class DisplayControl : public Register<u16, 0xFFF7>
    {
    public:
        template<uint Index>
        void write(u8 byte)
        {
            Register::write<Index>(byte);

            if (Index == 0)
            {
                mode     = bit::seq<0, 3>(byte);
                frame    = bit::seq<4, 1>(byte) * 0xA000;
                oam_free = bit::seq<5, 1>(byte);
                mapping  = bit::seq<6, 1>(byte);
                blank    = bit::seq<7, 1>(byte);
            }
            else
            {
                layers = bit::seq<0, 5>(byte);
                win0   = bit::seq<5, 1>(byte);
                win1   = bit::seq<6, 1>(byte);
                winobj = bit::seq<7, 1>(byte);
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

        uint mode     = 0;
        uint frame    = 0;
        uint oam_free = 0;
        uint mapping  = 0;
        uint blank    = config.bios_skip;
        uint layers   = 0;
        uint win0     = 0;
        uint win1     = 0;
        uint winobj   = 0;
    }
    dispcnt;

    Register<u16> greenswap;

    class DisplayStatus : public Register<u16, 0xFF38>
    {
    public:
        template<uint Index>
        u8 read() const
        {
            u8 value = Register::read<Index>();

            if (Index == 0)
            {
                value |= vblank << 0;
                value |= hblank << 1;
                value |= vmatch << 2;
            }
            return value;
        }

        template<uint Index>
        void write(u8 byte)
        {
            Register::write<Index>(byte);

            if (Index == 0)
            {
                vblank_irq = bit::seq<3, 1>(byte);
                hblank_irq = bit::seq<4, 1>(byte);
                vmatch_irq = bit::seq<5, 1>(byte);
            }
            else
            {
                vcompare = byte;
            }
        }

        uint vblank     = 0;
        uint hblank     = 0;
        uint vmatch     = 0;
        uint vblank_irq = 0;
        uint hblank_irq = 0;
        uint vmatch_irq = 0;
        uint vcompare   = 0;
    }
    dispstat;

    class VCount : public RegisterR<u16>
    {
    public:
        void next()
        {
            value = (value + 1) % 228;
        }
    }
    vcount;

    class BgControl : public Register<u16>
    {
    public:
        template<uint Index, uint Mask>
        void write(u8 byte)
        {
            Register::write<Index, Mask>(byte);

            if (Index == 0)
            {
                priority   = bit::seq<0, 2>(byte);
                tile_block = bit::seq<2, 2>(byte) * 0x4000;
                mosaic     = bit::seq<6, 1>(byte);
                color_mode = bit::seq<7, 1>(byte);
            }
            else
            {
                map_block  = bit::seq<0, 5>(byte) * 0x0800;
                wraparound = bit::seq<5, 1>(byte);
                dimensions = bit::seq<6, 2>(byte);
            }
        }

        Point dimsReg() const
        {
            return Point(
                256 << bit::seq<0, 1>(dimensions),
                256 << bit::seq<1, 1>(dimensions)
            );
        }

        Point dimsAff() const
        {
            return Point(
                128 << dimensions,
                128 << dimensions
            );
        }

        uint priority   = 0;
        uint tile_block = 0;
        uint mosaic     = 0;
        uint color_mode = 0;
        uint map_block  = 0;
        uint wraparound = 0;
        uint dimensions = 0;
    }
    bgcnt[4];

    RegisterW<u16, 0x01FF> bghofs[4];
    RegisterW<u16, 0x01FF> bgvofs[4];

    template<uint Init>
    class BgParameter : public RegisterW<s16>
    {
    public:
        static constexpr uint kInit = Init;

        BgParameter()
            : RegisterW(Init) {}
    };

    BgParameter<0x0100> bgpa[2];
    BgParameter<0x0000> bgpb[2];
    BgParameter<0x0000> bgpc[2];
    BgParameter<0x0100> bgpd[2];

    class BgReference : public RegisterW<u32, 0x0FFF'FFFF>
    {
    public:
        template<uint Index>
        void write(u8 byte)
        {
            RegisterW::write<Index>(byte);

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

        s32 current = 0;
    };

    BgReference bgx[2];
    BgReference bgy[2];

    class Window
    {
    public:
        void write(u8 byte)
        {
            flags = bit::seq<0, 5>(byte) | kLayerBdp;
            blend = bit::seq<5, 1>(byte);
        }

        uint flags = kLayerBdp;
        uint blend = 0;
    };

    class WindowInside : public Register<u16, 0x3F3F>
    {
    public:
        template<uint Index>
        void write(u8 byte)
        {
            Register::write<Index>(byte);

            if (Index == 0) win0.write(byte);
            if (Index == 1) win1.write(byte);
        }

        Window win0;
        Window win1;
    }
    winin;

    class WindowOutside : public Register<u16, 0x3F3F>
    {
    public:
        template<uint Index>
        void write(u8 byte)
        {
            Register::write<Index>(byte);

            if (Index == 0) winout.write(byte);
            if (Index == 1) winobj.write(byte);
        }

        Window winout;
        Window winobj;
    }
    winout;

    template<uint Max>
    class WindowRange : public RegisterW<u16>
    {
    public:
        static constexpr uint kMax = Max;

        template<uint Index>
        void write(u8 byte)
        {
            RegisterW::write<Index>(byte);

            max = data[0];
            min = data[1];

            if (max > Max || max < min)
                max = Max;
        }

        bool contains(uint value) const
        {
            return value >= min && value < max;
        }

        uint min = 0;
        uint max = 0;
    };

    WindowRange<kScreen.x> winh[2];
    WindowRange<kScreen.y> winv[2];

    class Mosaic
    {
    public:
        struct Block
        {
            void write(u8 byte)
            {
                x = bit::seq<0, 4>(byte) + 1;
                y = bit::seq<4, 4>(byte) + 1;
            }

            uint mosaicX(uint value) const { return x * (value / x); }
            uint mosaicY(uint value) const { return y * (value / y); }

            uint x = 1;
            uint y = 1;
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

    class BlendControl : public Register<u16, 0x3FFF>
    {
    public:
        template<uint Index>
        void write(u8 byte)
        {
            Register::write<Index>(byte);

            if (Index == 0)
            {
                upper = bit::seq<0, 6>(byte);
                mode  = bit::seq<6, 2>(byte);
            }
            else
            {
                lower = bit::seq<0, 6>(byte);
            }
        }

        uint mode  = 0;
        uint upper = 0;
        uint lower = 0;
    }
    bldcnt;

    class BlendAlpha : public Register<u16, 0x1F1F>
    {
    public:
        static constexpr uint kMaskR = 0x1F <<  0;
        static constexpr uint kMaskG = 0x1F <<  5;
        static constexpr uint kMaskB = 0x1F << 10;

        template<uint Index>
        void write(u8 byte)
        {
            Register::write<Index>(byte);

            if (Index == 0)
                eva = std::min<uint>(16, bit::seq<0, 5>(byte));
            else
                evb = std::min<uint>(16, bit::seq<0, 5>(byte));
        }

        u16 blendAlpha(u16 a, u16 b) const
        {
            uint rr = std::min(kMaskR, ((a & kMaskR) * eva + (b & kMaskR) * evb) >> 4);
            uint gg = std::min(kMaskG, ((a & kMaskG) * eva + (b & kMaskG) * evb) >> 4);
            uint bb = std::min(kMaskB, ((a & kMaskB) * eva + (b & kMaskB) * evb) >> 4);

            return (rr & kMaskR) | (gg & kMaskG) | (bb & kMaskB);
        }

        uint eva = 0;
        uint evb = 0;
    }
    bldalpha;

    class BlendFade : public RegisterW<u16, 0x001F>
    {
    public:
        static constexpr uint kMaskR = 0x1F <<  0;
        static constexpr uint kMaskG = 0x1F <<  5;
        static constexpr uint kMaskB = 0x1F << 10;

        template<uint Index>
        void write(u8 byte)
        {
            RegisterW::write<Index>(byte);

            if (Index == 0)
                evy = std::min<uint>(16, bit::seq<0, 5>(byte));
        }

        u16 blendWhite(u16 a) const
        {
            uint r = a & kMaskR;
            uint g = a & kMaskG;
            uint b = a & kMaskB;

            r += ((kMaskR - r) * evy) >> 4;
            g += ((kMaskG - g) * evy) >> 4;
            b += ((kMaskB - b) * evy) >> 4;

            return (r & kMaskR) | (g & kMaskG) | (b & kMaskB);
        }

        u16 blendBlack(u16 a) const
        {
            uint r = a & kMaskR;
            uint g = a & kMaskG;
            uint b = a & kMaskB;

            r -= (r * evy) >> 4;
            g -= (g * evy) >> 4;
            b -= (b * evy) >> 4;

            return (r & kMaskR) | (g & kMaskG) | (b & kMaskB);
        }

        uint evy = 0;
    }
    bldfade;
};
