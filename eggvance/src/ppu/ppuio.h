#pragma once

#include "registers/bgcontrol.h"
#include "registers/bgoffset.h"
#include "registers/bgparameter.h"
#include "registers/bgreference.h"
#include "registers/blendalpha.h"
#include "registers/blendcontrol.h"
#include "registers/blendfade.h"
#include "registers/displaycontrol.h"
#include "registers/displaystatus.h"
#include "registers/mosaic.h"
#include "registers/vcount.h"
#include "registers/window.h"
#include "registers/windowrange.h"

class PPUIO
{
public:
    inline void reset()
    {
        *this = PPUIO();
    }

    DisplayControl dispcnt;
    DisplayStatus dispstat;
    VCount vcount;

    BGControl bgcnt[4];
    BGOffset bghofs[4];
    BGOffset bgvofs[4];
    BGParameterA bgpa[2];
    BGParameterB bgpb[2];
    BGParameterC bgpc[2];
    BGParameterD bgpd[2];
    BGReference bgx[2];
    BGReference bgy[2];

    WindowInside winin;
    WindowOutside winout;
    WindowRangeHor winh[2];
    WindowRangeVer winv[2];

    Mosaic mosaic;
    BlendControl bldcnt;
    BlendAlpha bldalpha;
    BlendFade bldy;
};
