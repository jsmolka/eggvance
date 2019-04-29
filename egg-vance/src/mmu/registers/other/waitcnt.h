#pragma once

#include "../field.h"

class Waitcnt
{
public:
    Waitcnt(u16& data);

    u16& data;

    Field< 0, 2> sram;
    Field< 2, 2> nonseq0;
    Field< 4, 1> seq0;
    Field< 5, 2> nonseq1;
    Field< 7, 1> seq1;
    Field< 8, 2> nonseq2;
    Field<10, 1> seq2;
    Field<11, 2> phi;
    Field<14, 1> prefetch;
    Field<15, 1> type;
};

