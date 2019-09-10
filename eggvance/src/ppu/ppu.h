#pragma once

#include "mmu/mmu.h"
#include "backend.h"
#include "buffer.h"
#include "layers.h"
#include "objectdata.h"

class PPU
{
public:
    PPU(MMU& mmu);

    void reset();

    void scanline();
    void hblank();
    void vblank();
    void next();

    void present();

    Backend backend;

private:
    enum PixelFormat
    {
        BPP4,  // 4 bits per pixel (16/16)
        BPP8   // 8 bits per pixel (256/1)
    };
    MMU& mmu;
    MMIO& mmio;

    using RenderFunc = void(PPU::*)(int);

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

    template<int begin, int end>
    void collapse();
    template<bool has_objects>
    u16 findUpperLayer(const std::vector<Layer>& layers, int x);
    template<bool has_objects>
    void collapseNN(const std::vector<Layer>& layers);
    template<bool win0, bool win1, bool winobj>
    int windowFlags(int x);
    template<bool has_objects, bool win0, bool win1, bool winobj>
    u16 findUpperLayerWindow(const std::vector<Layer>& layers, int x, int flags);
    template<bool has_objects>
    void collapseNW(const std::vector<Layer>& layers);
    template<bool has_objects, bool win0, bool win1, bool winobj>
    void collapseNWImpl(const std::vector<Layer>& layers);
    template<bool has_objects>
    bool findBlendLayer(const std::vector<Layer>& layers, int x, int flags, u16& upper);
    template<bool has_objects>
    bool findBlendLayers(const std::vector<Layer>& layers, int x, int flags, u16& upper, u16& lower);
    template<bool has_objects>
    void collapseBN(const std::vector<Layer>& layers);
    template<bool has_objects, int blend_mode>
    void collapseBNImpl(const std::vector<Layer>& layers);
    template<bool has_objects>
    void collapseBW(const std::vector<Layer>& layers);
    template<bool has_objects, int blend_mode>
    void collapseBWImpl(const std::vector<Layer>& layers);
    template<bool win0, bool win1, bool winobj>
    void windowFlagsAndEffects(int x, int& flags, int& effects);
    template<bool has_objects, int blend_mode, bool win0, bool win1, bool winobj>
    void collapseBWImplImpl(const std::vector<Layer>& layers);

    int blendAlpha(int a, int b) const;
    int blendWhite(int a) const;
    int blendBlack(int a) const;

    int readBgColor(int index, int palette);
    int readFgColor(int index, int palette);
    int readPixel(u32 addr, int x, int y, PixelFormat format);
       
    DoubleBuffer<u16> bgs[4];
    Buffer<ObjectData> objects;
    bool obj_exist;
    bool obj_alpha;

    std::array<s16*, 32> pas;
    std::array<s16*, 32> pbs;
    std::array<s16*, 32> pcs;
    std::array<s16*, 32> pds;
};

#include "collapse.inl"
