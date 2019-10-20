#pragma once

#include "mmu/registers/bgcnt.h"
#include "mmu/registers/bgoffset.h"
#include "mmu/registers/bgparam.h"
#include "mmu/registers/bgref.h"
#include "mmu/registers/bldalpha.h"
#include "mmu/registers/bldcnt.h"
#include "mmu/registers/bldy.h"
#include "mmu/registers/dispcnt.h"
#include "mmu/registers/dispstat.h"
#include "mmu/registers/mosaic.h"
#include "mmu/registers/vcount.h"
#include "mmu/registers/windim.h"
#include "mmu/registers/window.h"

class PPUIO
{
public:
    void reset();

    DispCnt dispcnt;
    DispStat dispstat;
    VCount vcount;

    BgCnt bgcnt[4];
    BgOffset bghofs[4];
    BgOffset bgvofs[4];
    BgParam bgpa[2];
    BgParam bgpb[2];
    BgParam bgpc[2];
    BgParam bgpd[2];
    BgRef bgx[2];
    BgRef bgy[2];

    WinIn winin;
    WinOut winout;
    WinDim<240> winh[2];
    WinDim<160> winv[2];

    Mosaic mosaic;
    BldCnt bldcnt;
    BldAlpha bldalpha;
    BldY bldy;
};
