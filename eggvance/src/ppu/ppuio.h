#pragma once

#include "regs/bgcnt.h"
#include "regs/bgoffset.h"
#include "regs/bgparam.h"
#include "regs/bgref.h"
#include "regs/bldalpha.h"
#include "regs/bldcnt.h"
#include "regs/bldy.h"
#include "regs/dispcnt.h"
#include "regs/dispstat.h"
#include "regs/mosaic.h"
#include "regs/vcount.h"
#include "regs/windim.h"
#include "regs/window.h"

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
