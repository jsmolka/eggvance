#pragma once

#include "fs.h"
#include "input.h"

class Config
{
public:
    void init(const std::string& file);

    fs::path bios_file;
    fs::path save_dir;
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
    void initFile(const fs::path& file);
    void initDefault();
};

extern Config config;
