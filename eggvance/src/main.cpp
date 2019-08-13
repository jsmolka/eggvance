#include <filesystem>
#include <memory>

#include "core.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    fs::path exe = argv[0];
    fs::path dir = exe.parent_path();
    fs::path bios = dir.append("bios.bin");

    if (fs::is_regular_file(bios))
    {
        std::string file;
        if (argc > 1 && fs::is_regular_file(argv[1]))
            file = argv[1];

        std::make_shared<Core>()->run(bios.string(), file);
    }
    else
    {
        SDL_ShowSimpleMessageBox(
            0, 
            "Missing BIOS", 
            "Please place a GBA bios.bin in the emulator folder.", 
            nullptr
        );
    }
    return 0;
}
