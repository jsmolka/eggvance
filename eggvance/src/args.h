#pragma once

#include <string>

class Args
{
public:
    Args(int argc, char* argv[]);

    std::string exe;
    std::string dir;
    std::string rom;
};
