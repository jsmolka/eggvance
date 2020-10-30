#pragma once

#include <shell/buffer.h>

#include "buffer.h"
#include "io.h"
#include "layer.h"

class Gpu
{
public:
    Gpu();

    void scanline();
    void hblank();
    void vblank();
    void next();
    void present();

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

private:
    enum WindowFlag
    {
        kWindow0   = 1 << 0,
        kWindow1   = 1 << 1,
        kWindowObj = 1 << 2
    };

    using BgLayers = shell::FixedBuffer<BgLayer, 4>;
    using RenderFunc = void(Gpu::*)(uint);

    static u32 argb(u16 color);

    Point transform(uint x, uint bg);

    void renderBg(RenderFunc render, uint bg);
    void renderBgMode0(uint bg);
    void renderBgMode2(uint bg);
    void renderBgMode3(uint bg);
    void renderBgMode4(uint bg);
    void renderBgMode5(uint bg);
    void renderObjects();

    template<uint ColorMode>
    void renderBgMode0Impl(uint bg);

    void collapse(uint bgs);
    template<bool Objects>
    void collapse(const BgLayers& layers);
    template<bool Objects>
    void collapseNN(const BgLayers& layers);
    template<bool Objects>
    void collapseNW(const BgLayers& layers);
    template<bool Objects, uint Windows>
    void collapseNW(const BgLayers& layers);
    template<bool Objects>
    void collapseBN(const BgLayers& layers);
    template<bool Objects, uint BlendMode>
    void collapseBN(const BgLayers& layers);
    template<bool Objects>
    void collapseBW(const BgLayers& layers);
    template<bool Objects, uint BlendMode>
    void collapseBW(const BgLayers& layers);
    template<bool Objects, uint BlendMode, uint Windows>
    void collapseBW(const BgLayers& layers);

    template<bool Objects>
    uint possibleWindows() const;
    template<uint Windows>
    const Window& activeWindow(uint x) const;

    template<bool Objects>
    u16 upperLayer(const BgLayers& layers, uint x);
    template<bool Objects>
    u16 upperLayer(const BgLayers& layers, uint x, uint flags);
    template<bool Objects>
    bool findBlendLayers(const BgLayers& layers, uint x, uint flags, u16& upper);
    template<bool Objects>
    bool findBlendLayers(const BgLayers& layers, uint x, uint flags, u16& upper, u16& lower);

    ScanlineDoubleBuffer<u16> backgrounds[4];
    ScanlineBuffer<ObjectLayer> objects;
    bool objects_exist = false;
    bool objects_alpha = false;
};

inline Gpu gpu;
