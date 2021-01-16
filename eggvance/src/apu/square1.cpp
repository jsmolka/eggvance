#include "square1.h"

#include <shell/fmt.h>

#include "constants.h"

void Square1::tick()
{
    if (timer && --timer == 0)
    {
        constexpr int kWave[4] = { 
            0b00000001,
            0b00000011,
            0b00001111,
            0b00111111
        };

        if ((kWave[wave_duty] >> step) & 0x1)
            sample = 1;
        else
            sample = 0;

        sample <<= 4;

        //fmt::print("{:08X}\n", sample);

        step = (step + 1) % 8;

        timer = 16 * (2048 - frequency);// 128; //2 * (2048 - frequency);
        //fmt::print("{:08X}\n", timer);
    }
}
