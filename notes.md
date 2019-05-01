# Getting commercial games to run

## Kirby
- 0x8009200 (branched from 0x80091AE) used to render first frames
- 0x8002DB4 (branched from 0x8009284) used to render first frames
- 0x8002DC0 - 2DC8 loop for drawing the HAL logo
- 0x80022E4 - calls some drawing functions
- does not get passed function at 0x80022EE (at 0x8000DE4)
  - currently stuck at 0x8000FAE loop (hardware interrupt seems to be missing)