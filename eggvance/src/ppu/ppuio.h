#pragma once

#include "common/constants.h"
#include "regs/bgcontrol.h"
#include "regs/bgoffset.h"
#include "regs/bgparameter.h"
#include "regs/bgreference.h"
#include "regs/blendalpha.h"
#include "regs/blendcontrol.h"
#include "regs/blendfade.h"
#include "regs/displaycontrol.h"
#include "regs/displaystatus.h"
#include "regs/mosaic.h"
#include "regs/vcount.h"
#include "regs/window.h"
#include "regs/windowdimension.h"

class PPUIO
{
public:
    void reset();

    DisplayControl dispcnt;
    DisplayStatus dispstat;
    VCount vcount;

    BGControl bgcnt[4];
    BGOffset bghofs[4];
    BGOffset bgvofs[4];
    BGParameter bgpa[2];
    BGParameter bgpb[2];
    BGParameter bgpc[2];
    BGParameter bgpd[2];
    BGReference bgx[2];
    BGReference bgy[2];

    WindowIn winin;
    WindowOut winout;
    WindowDimension winh[2] = { SCREEN_W,  SCREEN_W  };
    WindowDimension winv[2] = { SCREEN_H, SCREEN_H };

    Mosaic mosaic;
    BlendControl bldcnt;
    BlendAlpha bldalpha;
    BlendFade bldy;
};
