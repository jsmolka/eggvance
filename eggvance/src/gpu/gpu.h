#pragma once

#include <vector>

#include "gpu/buffer.h"
#include "gpu/io.h"
#include "gpu/layer.h"

class Gpu
{
public:
    void reset();

    void scanline();
    void hblank();
    void vblank();
    void next();

    void present();

    PpuIo io;

private:
    enum WindowFlag
    {
        WF_WIN0   = 1 << 0,
        WF_WIN1   = 1 << 1,
        WF_WINOBJ = 1 << 2
    };

    using RenderFunc = void(Gpu::*)(int);

    Point transform(int x, int bg);

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
    void collapse(const std::vector<BGLayer>& layers);
    template<int obj_master>
    void collapseNN(const std::vector<BGLayer>& layers);
    template<int obj_master>
    void collapseNW(const std::vector<BGLayer>& layers);
    template<int obj_master, int win_master>
    void collapseNW(const std::vector<BGLayer>& layers);
    template<int obj_master>
    void collapseBN(const std::vector<BGLayer>& layers);
    template<int obj_master, int blend_mode>
    void collapseBN(const std::vector<BGLayer>& layers);
    template<int obj_master>
    void collapseBW(const std::vector<BGLayer>& layers);
    template<int obj_master, int blend_mode>
    void collapseBW(const std::vector<BGLayer>& layers);
    template<int obj_master, int blend_mode, int win_master>
    void collapseBW(const std::vector<BGLayer>& layers);

    template<int obj_master>
    int possibleWindows() const;
    template<int win_master>
    const PpuIo::Window& activeWindow(int x) const;

    template<int obj_master>
    u16 upperLayer(const std::vector<BGLayer>& layers, int x);
    template<int obj_master>
    u16 upperLayer(const std::vector<BGLayer>& layers, int x, int flags);

    template<int obj_master>
    bool findBlendLayers(const std::vector<BGLayer>& layers, int x, int flags, u16& upper);
    template<int obj_master>
    bool findBlendLayers(const std::vector<BGLayer>& layers, int x, int flags, u16& upper, u16& lower);

    static u32 argb(u16 color);

    DoubleBuffer<u16> backgrounds[4];
    Buffer<ObjectLayer> objects;
    bool objects_exist;
    bool objects_alpha;
};

inline Gpu gpu;
