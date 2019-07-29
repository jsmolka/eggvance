# Todo
- replace fast read methods and write methods (not needed)
- use pointers for displaying bitmap modes
  - mode 3 might just be a memcpy / copy_n
  - other modes can be simplified
- add cartridge and bios classes, parse cartridge header
- add proper descriptions to cpu tests / cleanup tests
- test if storing arm handler functions in a static lut is faster than switch
- test if using templates for flag heavy instructions helps with performance