#pragma once

#include "fs.h"
#include "input.h"

class Config
{
public:
    void init();

    Path bios_file;
    bool bios_skip;
    Path save_dir;
    int deadzone;

    struct Controls
    {
        InputConfig<Key> keyboard;
        InputConfig<Button> controller;
    } controls;

    struct Shortcuts
    {
        ShortcutConfig<Key> keyboard;
        ShortcutConfig<Button> controller;
    } shortcuts;

    double fps_multipliers[4];

private:
    void initFile();
    void initDefault();
};

extern Config config;
