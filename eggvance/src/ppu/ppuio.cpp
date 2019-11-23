#include "ppuio.h"

#include "common/config.h"

void PPUIO::reset()
{
    dispcnt.reset();
    dispstat.reset();
    vcount.reset();

    for (int x = 0; x < 4; ++x)
    {
        bgcnt[x].reset();
        bghofs[x].reset();
        bgvofs[x].reset();
    }

    for (int x = 0; x < 2; ++x)
    {
        bgx[x].reset();
        bgy[x].reset();
        bgpa[x].reset();
        bgpb[x].reset();
        bgpc[x].reset();
        bgpd[x].reset();
        winh[x].reset();
        winv[x].reset();
    }

    winin.reset();
    winout.reset();

    mosaic.reset();
    bldcnt.reset();
    bldalpha.reset();
    bldy.reset();

    if (config.bios_skip)
    {
        bgpa[0].value = 0x100;
        bgpa[1].value = 0x100;
        bgpd[0].value = 0x100;
        bgpd[1].value = 0x100;
    }
}
