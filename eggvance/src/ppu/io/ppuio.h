#pragma once

#include "bgcontrol.h"
#include "bgoffset.h"
#include "bgparameter.h"
#include "bgreference.h"
#include "blendalpha.h"
#include "blendcontrol.h"
#include "blendfade.h"
#include "displaycontrol.h"
#include "displaystatus.h"
#include "mosaic.h"
#include "vcount.h"
#include "window.h"
#include "windowrange.h"

struct PPUIO
{
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
