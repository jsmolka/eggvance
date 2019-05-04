#pragma once

#include "attribute0.h"
#include "attribute1.h"
#include "attribute2.h"

class OamEntry
{
public:
    OamEntry(u16 attr0, u16 attr1, u16 attr2);

    int spriteWidth() const;
    int spriteHeight() const;

    Attribute0 attr0;
    Attribute1 attr1;
    Attribute2 attr2;

private:
    static const int sprite_dims[4][4][2];
};

