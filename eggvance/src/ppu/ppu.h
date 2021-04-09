#pragma once

#include <shell/buffer.h>

#include "background.h"
#include "layers.h"
#include "oam.h"
#include "paletteram.h"
#include "videoram.h"
#include "scheduler/event.h"

class Ppu
{
public:
    void init();
    void present();

    DisplayControl dispcnt;
    Register<u16, 0x0001> greenswap;
    DisplayStatus dispstat;
    VCount vcount;

    shell::array<Background, 4> backgrounds = { 0, 1, 2, 3 };
    WindowInside winin;
    WindowOutside winout;
    WindowRange winh[2] = { kScreen.x, kScreen.x };
    WindowRange winv[2] = { kScreen.y, kScreen.y };
    Mosaic mosaic;
    BlendControl bldcnt;
    BlendAlpha bldalpha;
    BlendFade bldfade;

    PaletteRam pram = {};
    VideoRam vram = {};
    Oam oam = {};

private:
    using RenderFunc = void(Ppu::*)(Background&);
    using BackgroundLayers = shell::FixedBuffer<BgLayer, 4>;

    void scanline();
    void hblank(u64 late);
    void hblankEnd(u64 late);

    void renderBg(RenderFunc render, Background& background);
    void renderBgMode0(Background& background);
    void renderBgMode2(Background& background);
    void renderBgMode3(Background& background);
    void renderBgMode4(Background& background);
    void renderBgMode5(Background& background);
    void renderObjects();

    template<ColorMode kColorMode>
    void renderBgMode0Impl(Background& background);

    void collapse(uint bgs);
    template<bool kObjects                                     > void collapse(  const BackgroundLayers& backgrounds);
    template<bool kObjects                                     > void collapseNN(const BackgroundLayers& backgrounds);
    template<bool kObjects                                     > void collapseNW(const BackgroundLayers& backgrounds);
    template<bool kObjects,                       uint kWindows> void collapseNW(const BackgroundLayers& backgrounds);
    template<bool kObjects                                     > void collapseBN(const BackgroundLayers& backgrounds);
    template<bool kObjects, BlendMode kBlendMode               > void collapseBN(const BackgroundLayers& backgrounds);
    template<bool kObjects                                     > void collapseBW(const BackgroundLayers& backgrounds);
    template<bool kObjects, BlendMode kBlendMode               > void collapseBW(const BackgroundLayers& backgrounds);
    template<bool kObjects, BlendMode kBlendMode, uint kWindows> void collapseBW(const BackgroundLayers& backgrounds);

    template<bool kObjects> uint possibleWindows() const;
    template<uint kWindows> const Window& activeWindow(uint x) const;

    template<bool kObjects> u16  findUpperLayer(const BackgroundLayers& layers, uint x);
    template<bool kObjects> u16  findUpperLayer(const BackgroundLayers& layers, uint x, uint enabled);
    template<bool kObjects> bool findBlendLayer(const BackgroundLayers& layers, uint x, uint enabled, u16& upper);
    template<bool kObjects> bool findBlendLayer(const BackgroundLayers& layers, uint x, uint enabled, u16& upper, u16& lower);

    struct Events
    {
        Event hblank;
        Event hblank_end;
    } events;

    ScanlineBuffer<ObjectLayer> objects;
    bool objects_exist = false;
    bool objects_alpha = false;
};

inline Ppu ppu;
