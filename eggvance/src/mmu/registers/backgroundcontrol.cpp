#include "backgroundcontrol.h"

static constexpr int sizes[4][2] =
{
    { 256, 256 },
    { 512, 256 },
    { 256, 512 },
    { 512, 512 }
};

int BackgroundControl::size() const
{
    return 1 << (7 + screen_size);
}

int BackgroundControl::width() const
{
    return sizes[screen_size][0];
}

int BackgroundControl::height() const
{
    return sizes[screen_size][1];;
}
