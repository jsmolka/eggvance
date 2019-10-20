#include "args.h"

#include <filesystem>

namespace fs = std::filesystem;

Args::Args(int argc, char* argv[])
{
    if (argc > 0)
    {
        exe = argv[0];
        dir = fs::path(exe).parent_path().string();
    }
    if (argc > 1)
    {
        std::string rom = argv[1];
        if (fs::exists(rom) && fs::is_regular_file(rom))
            this->rom = rom;
    }
}
