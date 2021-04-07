#pragma once

#include <shell/buffer.h>

#include "buffer.h"
#include "io.h"
#include "layers.h"
#include "oam.h"
#include "paletteram.h"
#include "videoram.h"
#include "scheduler/event.h"

class Ppu
{
public:
    friend class Arm;

    Ppu();

    void init();
    void present();

    DisplayControl dispcnt;
    Register<u16, 0x0001> greenswap;
    DisplayStatus dispstat;
    VCount vcount;
    BgControl bgcnt[4] = { 0, 1, 2, 3 };
    BgOffset bghofs[4];
    BgOffset bgvofs[4];
    TransformationMatrix matrix[2];
    WindowInside winin;
    WindowOutside winout;
    WindowRange winh[2] = { kScreen.x, kScreen.x };
    WindowRange winv[2] = { kScreen.y, kScreen.y };
    Mosaic mosaic;
    BlendControl bldcnt;
    BlendAlpha bldalpha;
    BlendFade bldfade;

private:
    using RenderFunc = void(Ppu::*)(uint);
    using BackgroundLayers = shell::FixedBuffer<BgLayer, 4>;

    void scanline();
    void hblank(u64 late);
    void hblankEnd(u64 late);

    void renderBg(RenderFunc render, uint bg);
    void renderBgMode0(uint bg);
    void renderBgMode2(uint bg);
    void renderBgMode3(uint bg);
    void renderBgMode4(uint bg);
    void renderBgMode5(uint bg);
    void renderObjects();

    template<ColorMode kColorMode>
    void renderBgMode0Impl(uint bg);

    void collapse(uint bgs);
    template<bool kObjects> void collapse(const BackgroundLayers& backgrounds);
    template<bool kObjects> void collapseNN(const BackgroundLayers& backgrounds);
    template<bool kObjects> void collapseNW(const BackgroundLayers& backgrounds);
    template<bool kObjects, uint kWindows> void collapseNW(const BackgroundLayers& backgrounds);
    template<bool kObjects> void collapseBN(const BackgroundLayers& backgrounds);
    template<bool kObjects, BlendMode kBlendMode> void collapseBN(const BackgroundLayers& backgrounds);
    template<bool kObjects> void collapseBW(const BackgroundLayers& backgrounds);
    template<bool kObjects, BlendMode kBlendMode> void collapseBW(const BackgroundLayers& backgrounds);
    template<bool kObjects, BlendMode kBlendMode, uint kWindows> void collapseBW(const BackgroundLayers& backgrounds);

    template<bool kObjects> uint possibleWindows() const;
    template<uint kWindows> const Window& activeWindow(uint x) const;

    template<bool kObjects> u16 upperLayer(const BackgroundLayers& backgrounds, uint x);
    template<bool kObjects> u16 upperLayer(const BackgroundLayers& backgrounds, uint x, uint enabled);
    template<bool kObjects> bool findBlendLayers(const BackgroundLayers& backgrounds, uint x, uint enabled, u16& upper);
    template<bool kObjects> bool findBlendLayers(const BackgroundLayers& backgrounds, uint x, uint enabled, u16& upper, u16& lower);

    struct
    {
        Event hblank;
        Event hblank_end;
    } events;

    ScanlineBuffer<ObjectLayer> objects;
    ScanlineDoubleBuffer<u16> backgrounds[4];
    bool objects_exist = false;
    bool objects_alpha = false;

    PaletteRam pram = {};
    VideoRam vram = {};
    Oam oam = {};
    shell::array<u32, 0x8000> argb;
};

inline Ppu ppu;
