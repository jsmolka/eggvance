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

    DisplayControl dispcnt;
    Register<u16, 0x0001> greenswap;
    DisplayStatus dispstat;
    VerticalCounter vcount;
    shell::array<Background, 4> backgrounds = { 0, 1, 2, 3 };

    WindowInside winin;
    WindowOutside winout;
    shell::array<WindowRange, 2> winh = { kScreenW, kScreenW };
    shell::array<WindowRange, 2> winv = { kScreenH, kScreenH };

    Mosaic mosaic;
    BlendControl bldcnt;
    BlendAlpha bldalpha;
    BlendFade bldfade;

    PaletteRam pram = {};
    VideoRam vram = {};
    Oam oam = {};

private:
    struct BlendLayer
    {
        uint flag  = 0;
        uint color = 0;
    };

    using BlendLayers      = std::pair<BlendLayer, BlendLayer>;
    using BackgroundRender = void(Ppu::*)(Background&);
    using BackgroundLayers = shell::FixedBuffer<BackgroundLayer, 4>;

    void hblank(u64 late);
    void hblankEnd(u64 late);

    void render();
    void renderBackground(BackgroundRender render, Background& background);
    void renderObjects();

    template<uint kColorMode>
    void renderBackground0(Background& background);
    void renderBackground0(Background& background);
    void renderBackground2(Background& background);
    void renderBackground3(Background& background);
    void renderBackground4(Background& background);
    void renderBackground5(Background& background);

    void compose(uint possible);
    template<bool kObjects>
    void composeNN(const BackgroundLayers& layers);
    template<bool kObjects, uint kWindows>
    void composeNW(const BackgroundLayers& layers);
    template<bool kObjects, uint kBlendMode>
    void composeBN(const BackgroundLayers& layers);
    template<bool kObjects, uint kBlendMode, uint kWindows>
    void composeBW(const BackgroundLayers& layers);

    template<uint kWindows>
    const Window& activeWindow(uint x) const;

    template<bool kObjects>
    u16 upperLayer(const BackgroundLayers& layers, uint x, uint enabled);
    template<bool kObjects>
    BlendLayers blendLayers(const BackgroundLayers& layers, uint x, uint enabled);

    uint objects_exist = false;
    uint objects_alpha = false;
    ScanlineBuffer<ObjectLayer> objects;

    struct Events
    {
        Event hblank;
        Event hblank_end;
    } events;
};

inline Ppu ppu;
