#pragma once

#include <array>
#include <string>

#include "common/integer.h"
#include "registers/include.h"

class MMU
{
public:
    MMU();

    void reset();

    bool readFile(const std::string& file, u32 addr);

    void dump(u32 start, u32 size);

    void requestInterrupt(InterruptFlag flag);

    u8  readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    u8  readByteFast(u32 addr);
    u16 readHalfFast(u32 addr);
    u32 readWordFast(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    Dispcnt dispcnt;
    Dispstat dispstat;
    Vcount vcount;
    union
    {
        struct
        {
            Bgcnt bg0cnt;
            Bgcnt bg1cnt;
            Bgcnt bg2cnt;
            Bgcnt bg3cnt;
        };
        Bgcnt bgcnt[4];
    };
    union
    {
        struct
        {
            Bghofs bg0hofs;
            Bghofs bg1hofs;
            Bghofs bg2hofs;
            Bghofs bg3hofs;
        };
        Bghofs bghofs[4];
    };
    union
    {
        struct
        {
            Bgvofs bg0vofs;
            Bgvofs bg1vofs;
            Bgvofs bg2vofs;
            Bgvofs bg3vofs;
        };
        Bgvofs bgvofs[4];
    };
    union
    {
        struct
        {
            Bgx bg2x;
            Bgx bg3x;
        };
        Bgx bgx[2];
    };
    union
    {
        struct
        {
            Bgy bg2y;
            Bgy bg3y;
        };
        Bgy bgy[2];
    };
    union
    {
        struct
        {
            Bgpa bg2pa;
            Bgpa bg3pa;
        };
        Bgpa bgpa[2];
    };
    union
    {
        struct
        {
            Bgpb bg2pb;
            Bgpb bg3pb;
        };
        Bgpb bgpb[2];
    };
    union
    {
        struct
        {
            Bgpc bg2pc;
            Bgpc bg3pc;
        };
        Bgpc bgpc[2];
    };
    union
    {
        struct
        {
            Bgpa bg2pd;
            Bgpd bg3pd;
        };
        Bgpd bgpd[2];
    };
    union
    {
        struct
        {
            Winh win0h;
            Winh win1h;
        };
        Winh winh[2];
    };
    union
    {
        struct
        {
            Winv win0v;
            Winv win1v;
        };
        Winv winv[2];
    };
    Winin winin;
    Winout winout;
    Mosaic mosaic;
    Bldcnt bldcnt;
    Bldalpha bldalpha;
    Bldy bldy;
    Keycnt keycnt;
    Keyinput keyinput;
    Waitcnt waitcnt;
    InterruptMaster int_master;
    InterruptEnabled int_enabled;
    InterruptRequest int_request;

    bool halt;

private:
    template<typename T>
    T& ref(u32 addr);

    std::array<u8, 0x10000000> memory;
};
