#pragma once

#include "base/fs.h"
#include "base/input.h"

class Config
{
public:
    void init(int argc, char* argv[]);

    struct
    {
        Controls<Key> keyboard;
        Controls<Button> controller;
    } controls;

    struct
    {
        Shortcuts<Key> keyboard;
        Shortcuts<Button> controller;
    } shortcuts;

    double framerate[4];
    fs::path save_path;
    fs::path bios_file;
    bool bios_skip;

    std::string save_type;
    std::string gpio_type;

private:
    void initFile(const fs::path& file);
    void initDefault();
};

extern Config config;
