#pragma once

#include "attribute0.h"
#include "attribute1.h"
#include "attribute2.h"

class OamEntry
{
public:
    OamEntry(int attr0, int attr1, int attr2);

    int width() const;
    int height() const;

    Attribute0 attr0;
    Attribute1 attr1;
    Attribute2 attr2;

private:
    static const int sprite_dims[4][4][2];
};

