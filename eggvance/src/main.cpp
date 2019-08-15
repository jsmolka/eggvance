#include <filesystem>

#include "core.h"

namespace fs = std::filesystem;

void exit(const char* title, const char* message)
{
    SDL_ShowSimpleMessageBox(0, title, message, nullptr);
    std::exit(1);
}

int main(int argc, char* argv[])
{
    fs::path directory = fs::path(argv[0]).parent_path();
    fs::path bios_file = directory.append("bios.bin");

    if (!fs::is_regular_file(bios_file))
        exit("Missing BIOS", "Please place a GBA bios.bin next to the emulator.");

    auto bios = std::make_shared<BIOS>(bios_file.string());
    if (!bios->valid)
        exit("Invalid BIOS", "The BIOS does not match the requirements.");

    std::shared_ptr<GamePak> gamepak;
    if (argc > 1 && fs::is_regular_file(argv[1]))
        gamepak = std::make_shared<GamePak>(argv[1]);

    std::make_unique<Core>(bios)->run(gamepak);

    return 0;
}
