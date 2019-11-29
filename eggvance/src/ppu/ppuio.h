#pragma once

#include "common/constants.h"
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
#include "registers/windowdimension.h"

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
