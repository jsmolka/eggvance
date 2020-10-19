#pragma once

#include <shell/iterator.h>

#include "buffer.h"
#include "io.h"
#include "layer.h"

class Gpu
{
public:
    friend class Io;
    friend class Vram;

    Gpu();

    void scanline();
    void hblank();
    void vblank();
    void next();

    void present();

private:
    enum WindowFlag
    {
        kWindow0   = 1 << 0,
        kWindow1   = 1 << 1,
        kWindowObj = 1 << 2
    };

    using BgLayers = shell::IteratorRange<const BgLayer*>;

    using RenderFunc = void(Gpu::*)(int);

    Point transform(int x, int bg);

    void renderBg(RenderFunc func, int bg);
    void renderBgMode0(int bg);
    void renderBgMode2(int bg);
    void renderBgMode3(int bg);
    void renderBgMode4(int bg);
    void renderBgMode5(int bg);
    void renderObjects();

    void mosaicBg(int bg);
    bool mosaicAffected(int bg) const;
    bool mosaicYDominant() const;

    void collapse(int begin, int end);
    template<uint Objects>
    void collapse(const BgLayers& layers);
    template<uint Objects>
    void collapseNN(const BgLayers& layers);
    template<uint Objects>
    void collapseNW(const BgLayers& layers);
    template<uint Objects, uint Windows>
    void collapseNW(const BgLayers& layers);
    template<uint Objects>
    void collapseBN(const BgLayers& layers);
    template<uint Objects, uint BlendMode>
    void collapseBN(const BgLayers& layers);
    template<uint Objects>
    void collapseBW(const BgLayers& layers);
    template<uint Objects, uint BlendMode>
    void collapseBW(const BgLayers& layers);
    template<uint Objects, uint BlendMode, uint Windows>
    void collapseBW(const BgLayers& layers);

    template<uint Objects>
    uint possibleWindows() const;
    template<uint Windows>
    const Window& activeWindow(uint x) const;

    template<uint Objects> u16 upperLayer(const BgLayers& layers, uint x);
    template<uint Objects> u16 upperLayer(const BgLayers& layers, uint x, uint flags);
    template<uint Objects> bool findBlendLayers(const BgLayers& layers, uint x, uint flags, u16& upper);
    template<uint Objects> bool findBlendLayers(const BgLayers& layers, uint x, uint flags, u16& upper, u16& lower);

    static u32 argb(u16 color);

    DoubleBuffer<u16> backgrounds[4];
    Buffer<ObjectLayer> objects;
    bool objects_exist = false;
    bool objects_alpha = false;

    DisplayControl dispcnt;
    Register<u16> greenswap;
    DisplayStatus dispstat;
    VCount vcount;
    BgControl bgcnt[4];
    RegisterW<u16, 0x01FF> bghofs[4];
    RegisterW<u16, 0x01FF> bgvofs[4];
    BgParameter<0x0100> bgpa[2];
    BgParameter<0x0000> bgpb[2];
    BgParameter<0x0000> bgpc[2];
    BgParameter<0x0100> bgpd[2];
    BgReference bgx[2];
    BgReference bgy[2];
    WindowInside winin;
    WindowOutside winout;
    WindowRange<kScreen.x> winh[2];
    WindowRange<kScreen.y> winv[2];
    Mosaic mosaic;
    BlendControl bldcnt;
    BlendAlpha bldalpha;
    BlendFade bldfade;
};

inline Gpu gpu;
