#pragma once

#include <string>

class Config
{
public:
    void init();

    bool bios_skip = true;
    std::string bios_file = "bios.bin";
};

extern Config config;
