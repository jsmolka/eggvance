#include "oamentry.h"

OamEntry::OamEntry(int attr0, int attr1, int attr2)
    : attr0(attr0)
    , attr1(attr1)
    , attr2(attr2)
{

}

int OamEntry::spriteWidth() const
{
    return sprite_dims[attr0.obj_shape][attr1.obj_size][0];
}

int OamEntry::spriteHeight() const
{
    return sprite_dims[attr0.obj_shape][attr1.obj_size][1];
}

const int OamEntry::sprite_dims[4][4][2] = // [shape][size][width, height]
{
    // Shape 00
    {
        {  8,  8 },  // Size 00
        { 16, 16 },  // Size 01
        { 32, 32 },  // Size 10
        { 64, 64 },  // Size 11

    },
    // Shape 01
    {
        { 16,  8 },
        { 32,  8 },
        { 32, 16 },
        { 64, 32 },
    },
    // Shape 10
    {
        {  8, 16 },
        {  8, 32 },
        { 16, 32 },
        { 32, 64 },
    },
    // Shape 11 (prohibited)
    {
        {  0,  0 },
        {  0,  0 },
        {  0,  0 },
        {  0,  0 }
    }
};
