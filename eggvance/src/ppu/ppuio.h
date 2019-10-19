#pragma once

#include "mmu/registers/blendalpha.h"
#include "mmu/registers/blendbrightness.h"
#include "mmu/registers/blendcontrol.h"
#include "mmu/registers/backgroundcontrol.h"
#include "mmu/registers/backgroundoffset.h"
#include "mmu/registers/backgroundparameter.h"
#include "mmu/registers/backgroundreference.h"
#include "mmu/registers/displaycontrol.h"
#include "mmu/registers/displaystatus.h"
#include "mmu/registers/mosaic.h"
#include "mmu/registers/window.h"
#include "mmu/registers/windowrange.h"

class PPUIO
{
public:
    void reset();

    DisplayControl dispcnt;
    DisplayStatus dispstat;
    int vcount;

    BackgroundControl bgcnt[4];
    BackgroundOffset bghofs[4];
    BackgroundOffset bgvofs[4];
    BackgroundReference bgx[2];
    BackgroundReference bgy[2];
    BackgroundParameter bgpa[2];
    BackgroundParameter bgpb[2];
    BackgroundParameter bgpc[2];
    BackgroundParameter bgpd[2];

    WindowInside winin;
    WindowOutside winout;
    WindowRange<240> winh[2];
    WindowRange<160> winv[2];

    Mosaic mosaic;
    BlendControl bldcnt;
    BlendAlpha bldalpha;
    BlendBrightness bldy;
};
