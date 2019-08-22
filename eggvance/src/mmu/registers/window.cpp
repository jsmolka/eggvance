#include "window.h"

#include "common/utility.h"

void Window::write(u8 byte)
{
    bg0 = bits<0, 1>(byte);
    bg1 = bits<1, 1>(byte);
    bg2 = bits<2, 1>(byte);
    bg3 = bits<3, 1>(byte);
    obj = bits<4, 1>(byte);
    sfx = bits<5, 1>(byte);
}
