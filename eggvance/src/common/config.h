#pragma once

#include <string>

#include "common/input.h"

class Config
{
public:
    void init();

    std::string bios_file;
    std::string save_dir;
    bool bios_skip;
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
