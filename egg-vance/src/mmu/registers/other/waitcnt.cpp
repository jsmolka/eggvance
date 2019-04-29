#include "waitcnt.h"

Waitcnt::Waitcnt(u16& data)
    : data(data)
    , sram(data)
    , nonseq0(data)
    , seq0(data)
    , nonseq1(data)
    , seq1(data)
    , nonseq2(data)
    , seq2(data)
    , phi(data)
    , prefetch(data)
    , type(data)
{

}
