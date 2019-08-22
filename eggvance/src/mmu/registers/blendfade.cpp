#include "blendfade.h"

#include "common/utility.h"

void BlendFade::write(u8 byte)
{
    evy = bits<0, 5>(byte);
}
