#include "oamentry.h"

static constexpr int sizes[4][4][2] = 
{
    {
        {  8,  8 },
        { 16, 16 },
        { 32, 32 },
        { 64, 64 },
    },
    {
        { 16,  8 },
        { 32,  8 },
        { 32, 16 },
        { 64, 32 },
    },
    {
        {  8, 16 },
        {  8, 32 },
        { 16, 32 },
        { 32, 64 },
    },
    {
        {  0,  0 },
        {  0,  0 },
        {  0,  0 },
        {  0,  0 }
    }
};

int OAMEntry::width() const
{
    return sizes[shape][size][0];
}

int OAMEntry::height() const
{
    return sizes[shape][size][1];
}
