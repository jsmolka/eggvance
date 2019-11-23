#pragma once

#include <vector>

#include "buffer.h"
#include "dimensions.h"
#include "layers.h"
#include "ppuio.h"
#include "videobackend.h"

class PPU
{
public:
    void reset();

    void scanline();
    void hblank();
    void vblank();
    void next();

    void present();

    VideoBackend backend;

    PPUIO io;

private:
    enum WindowFlag
    {
        WF_WIN0   = 1 << 0,
        WF_WIN1   = 1 << 1,
        WF_WINOBJ = 1 << 2
    };

    using RenderFunc = void(PPU::*)(int);

    Point transformBG(int x, int bg) const;

    void renderBg(RenderFunc func, int bg);
    void renderBgMode0(int bg);
    void renderBgMode2(int bg);
    void renderBgMode3(int bg);
    void renderBgMode4(int bg);
    void renderBgMode5(int bg);
    void renderObjects();

    void mosaic(int bg);
    bool mosaicAffected(int bg) const;
    bool mosaicDominant() const;

    void collapse(int begin, int end);
    template<int obj_master>
    void collapse(const std::vector<BackgroundLayer>& layers);
    template<int obj_master>
    void collapseNN(const std::vector<BackgroundLayer>& layers);
    template<int obj_master>
    void collapseNW(const std::vector<BackgroundLayer>& layers);
    template<int obj_master, int win_master>
    void collapseNW(const std::vector<BackgroundLayer>& layers);
    template<int obj_master>
    void collapseBN(const std::vector<BackgroundLayer>& layers);
    template<int obj_master, int blend_mode>
    void collapseBN(const std::vector<BackgroundLayer>& layers);
    template<int obj_master>
    void collapseBW(const std::vector<BackgroundLayer>& layers);
    template<int obj_master, int blend_mode>
    void collapseBW(const std::vector<BackgroundLayer>& layers);
    template<int obj_master, int blend_mode, int win_master>
    void collapseBW(const std::vector<BackgroundLayer>& layers);

    template<int obj_master>
    int possibleWindows() const;
    template<int win_master>
    const Window& activeWindow(int x) const;

    template<int obj_master>
    u16 upperLayer(const std::vector<BackgroundLayer>& layers, int x);
    template<int obj_master>
    u16 upperLayer(const std::vector<BackgroundLayer>& layers, int x, int flags);

    template<int obj_master>
    bool findBlendLayers(const std::vector<BackgroundLayer>& layers, int x, int flags, u16& upper);
    template<int obj_master>
    bool findBlendLayers(const std::vector<BackgroundLayer>& layers, int x, int flags, u16& upper, u16& lower);

    u16 blendAlpha(u16 a, u16 b) const;
    u16 blendWhite(u16 a) const;
    u16 blendBlack(u16 a) const;

    static u32 argb(u16 color);

    DoubleBuffer<u16> backgrounds[4];
    Buffer<ObjectLayer> objects;
    bool objects_exist;
    bool objects_alpha;
};

extern PPU ppu;
