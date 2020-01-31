#include "intrmaster.h"

#include <cstring>

IntrMaster::IntrMaster()
{
    std::memset(data, 0, sizeof(data));

    master = false;
}

void IntrMaster::reset()
{
    *this = IntrMaster();
}
