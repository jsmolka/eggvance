#pragma once

#include "base/fs.h"
#include "base/input.h"

class Config
{
public:
    void init(int argc, char* argv[]);

    fs::path save_path;
    fs::path bios_file;
    bool bios_skip;

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

    double framerate[4];

private:
    void initFile(const fs::path& file);
    void initDefault();

    fs::path parent;
};

extern Config config;
