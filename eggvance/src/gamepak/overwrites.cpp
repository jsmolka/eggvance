#include "overwrites.h"

#include <optional>
#include <string_view>
#include <tuple>

std::optional<Overwrite> Overwrite::find(const std::string& game_code)
{
    static constexpr std::tuple<std::string_view, Overwrite> kOverwrites[] =
    {
        { "AWRE", { Save::Type::Flash512,  Gpio::Type::None, false } },  // Advance Wars
        { "AWRP", { Save::Type::Flash512,  Gpio::Type::None, false } },  // Advance Wars
        { "AW2E", { Save::Type::Flash512,  Gpio::Type::None, false } },  // Advance Wars 2: Black Hole Rising
        { "AW2P", { Save::Type::Flash512,  Gpio::Type::None, false } },  // Advance Wars 2: Black Hole Rising
        { "U3IJ", { Save::Type::Eeprom,    Gpio::Type::Rtc,  false } },  // Boktai: The Sun is in Your Hand
        { "U3IE", { Save::Type::Eeprom,    Gpio::Type::Rtc,  false } },  // Boktai: The Sun is in Your Hand
        { "U3IP", { Save::Type::Eeprom,    Gpio::Type::Rtc,  false } },  // Boktai: The Sun is in Your Hand
        { "U32J", { Save::Type::Eeprom,    Gpio::Type::Rtc,  false } },  // Boktai 2: Solar Boy Django
        { "U32E", { Save::Type::Eeprom,    Gpio::Type::Rtc,  false } },  // Boktai 2: Solar Boy Django
        { "U32P", { Save::Type::Eeprom,    Gpio::Type::Rtc,  false } },  // Boktai 2: Solar Boy Django
        { "FADE", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Castlevania
        { "FBME", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Bomberman
        { "FDKE", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Donkey Kong
        { "FDME", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Dr. Mario
        { "FEBE", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Excitebike
        { "FICE", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Ice Climber
        { "FLBE", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Zelda II - The Adventure of Link
        { "FMRE", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Metroid
        { "FP7E", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Pac-Man
        { "FSME", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Super Mario Bros.
        { "FXVE", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Xevious
        { "FZLE", { Save::Type::Eeprom,    Gpio::Type::Rtc,  true  } },  // Classic NES Series - Legend of Zelda
        { "AC8J", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Crash Bandicoot 2 - N-Tranced
        { "AC8E", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Crash Bandicoot 2 - N-Tranced
        { "AC8P", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Crash Bandicoot 2 - N-Tranced
        { "ALGP", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Dragon Ball Z - The Legacy of Goku
        { "ALFJ", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Dragon Ball Z - The Legacy of Goku II
        { "ALFE", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Dragon Ball Z - The Legacy of Goku II
        { "ALFP", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Dragon Ball Z - The Legacy of Goku II
        { "BDBE", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Dragon Ball Z - Taiketsu
        { "BDBP", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Dragon Ball Z - Taiketsu
        { "V49J", { Save::Type::Sram,      Gpio::Type::None, false } },  // Drill Dozer
        { "V49E", { Save::Type::Sram,      Gpio::Type::None, false } },  // Drill Dozer
        { "PEAJ", { Save::Type::Flash1024, Gpio::Type::None, false } },  // e-Reader
        { "PSAJ", { Save::Type::Flash1024, Gpio::Type::None, false } },  // e-Reader
        { "PSAE", { Save::Type::Flash1024, Gpio::Type::None, false } },  // e-Reader
        { "FMBJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 01 - Super Mario Bros.
        { "FCLJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 12 - Clu Clu Land
        { "FBFJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 13 - Balloon Fight
        { "FWCJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 14 - Wrecking Crew
        { "FDMJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 15 - Dr. Mario
        { "FDDJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 16 - Dig Dug
        { "FTBJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 17 - Takahashi Meijin no Boukenjima
        { "FMKJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 18 - Makaimura
        { "FTWJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 19 - Twin Bee
        { "FGGJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 20 - Ganbare Goemon! Karakuri Douchuu
        { "FM2J", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 21 - Super Mario Bros. 2
        { "FNMJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 22 - Nazo no Murasame Jou
        { "FMRJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 23 - Metroid
        { "FPTJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 24 - Hikari Shinwa - Palthena no Kagami
        { "FLBJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 25 - The Legend of Zelda 2 - Link no Bouken
        { "FFMJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 26 - Famicom Mukashi Banashi - Shin Onigashima - Zen Kou Hen
        { "FTKJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 27 - Famicom Tantei Club - Kieta Koukeisha - Zen Kou Hen
        { "FTUJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 28 - Famicom Tantei Club Part II - Ushiro ni Tatsu Shoujo - Zen Kou Hen
        { "FADJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 29 - Akumajou Dracula
        { "FSDJ", { Save::Type::Eeprom,    Gpio::Type::None, true  } },  // Famicom Mini Vol. 30 - SD Gundam World - Gachapon Senshi Scramble Wars
        { "AFXE", { Save::Type::Flash512,  Gpio::Type::None, false } },  // Final Fantasy Tactics Advance
        { "BFTJ", { Save::Type::Flash1024, Gpio::Type::None, false } },  // F-Zero - Climax
        { "AGFE", { Save::Type::Flash512,  Gpio::Type::None, false } },  // Golden Sun: The Lost Age
        { "AI2E", { Save::Type::None,      Gpio::Type::None, false } },  // Iridion II
        { "AI2P", { Save::Type::None,      Gpio::Type::None, false } },  // Iridion II
        { "KHPJ", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Koro Koro Puzzle - Happy Panechu!
        { "BLJJ", { Save::Type::Flash512,  Gpio::Type::Rtc,  false } },  // Legendz - Yomigaeru Shiren no Shima
        { "BLJK", { Save::Type::Flash512,  Gpio::Type::Rtc,  false } },  // Legendz - Yomigaeru Shiren no Shima KOR
        { "BLVJ", { Save::Type::Flash512,  Gpio::Type::Rtc,  false } },  // Legendz - Sign of Nekuromu
        { "AREE", { Save::Type::Sram,      Gpio::Type::None, false } },  // Mega Man Battle Network
        { "AZCE", { Save::Type::Sram,      Gpio::Type::None, false } },  // Mega Man Zero
        { "BSME", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Metal Slug Advance
        { "BPPJ", { Save::Type::Sram,      Gpio::Type::None, false } },  // Pokemon Pinball: Ruby & Sapphire
        { "BPPE", { Save::Type::Sram,      Gpio::Type::None, false } },  // Pokemon Pinball: Ruby & Sapphire
        { "BPPP", { Save::Type::Sram,      Gpio::Type::None, false } },  // Pokemon Pinball: Ruby & Sapphire
        { "BPPU", { Save::Type::Sram,      Gpio::Type::None, false } },  // Pokemon Pinball: Ruby & Sapphire
        { "AXVJ", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Ruby
        { "AXVE", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Ruby
        { "AXVP", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Ruby
        { "AXVI", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Ruby
        { "AXVS", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Ruby
        { "AXVD", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Ruby
        { "AXVF", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Ruby
        { "AXPJ", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Sapphire
        { "AXPE", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Sapphire
        { "AXPP", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Sapphire
        { "AXPI", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Sapphire
        { "AXPS", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Sapphire
        { "AXPD", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Sapphire
        { "AXPF", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Sapphire
        { "BPEJ", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Emerald
        { "BPEE", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Emerald
        { "BPEP", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Emerald
        { "BPEI", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Emerald
        { "BPES", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Emerald
        { "BPED", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Emerald
        { "BPEF", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Pokemon Emerald
        { "B24J", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon Mystery Dungeon
        { "B24E", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon Mystery Dungeon
        { "B24P", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon Mystery Dungeon
        { "B24U", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon Mystery Dungeon
        { "BPRJ", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon FireRed
        { "BPRE", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon FireRed
        { "BPRP", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon FireRed
        { "BPRI", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon FireRed
        { "BPRS", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon FireRed
        { "BPRD", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon FireRed
        { "BPRF", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon FireRed
        { "BPGJ", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon LeafGreen
        { "BPGE", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon LeafGreen
        { "BPGP", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon LeafGreen
        { "BPGI", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon LeafGreen
        { "BPGS", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon LeafGreen
        { "BPGD", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon LeafGreen
        { "BPGF", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Pokemon LeafGreen
        { "BR4J", { Save::Type::Flash512,  Gpio::Type::Rtc,  false } },  // RockMan EXE 4.5 - Real Operation
        { "AR8E", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Rocky
        { "AROP", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Rocky
        { "BKAJ", { Save::Type::Flash1024, Gpio::Type::Rtc,  false } },  // Sennen Kazoku
        { "U33J", { Save::Type::Eeprom,    Gpio::Type::Rtc,  false } },  // Shin Bokura no Taiyou: Gyakushuu no Sabata
        { "AA2J", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Super Mario Advance 2
        { "AA2E", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Super Mario Advance 2
        { "A3AJ", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Super Mario Advance 3
        { "A3AE", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Super Mario Advance 3
        { "A3AP", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Super Mario Advance 3
        { "AX4J", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Super Mario Advance 4
        { "AX4E", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Super Mario Advance 4
        { "AX4P", { Save::Type::Flash1024, Gpio::Type::None, false } },  // Super Mario Advance 4
        { "ALUE", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Super Monkey Ball Jr.
        { "ALUP", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Super Monkey Ball Jr.
        { "A2YE", { Save::Type::None,      Gpio::Type::None, false } },  // Top Gun - Combat Zones
        { "BUHJ", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Ueki no Housoku - Jingi Sakuretsu! Nouryokusha Battle
        { "RZWJ", { Save::Type::Sram,      Gpio::Type::None, false } },  // Wario Ware Twisted
        { "RZWE", { Save::Type::Sram,      Gpio::Type::None, false } },  // Wario Ware Twisted
        { "RZWP", { Save::Type::Sram,      Gpio::Type::None, false } },  // Wario Ware Twisted
        { "KYGJ", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Yoshi's Universal Gravitation
        { "KYGE", { Save::Type::Eeprom,    Gpio::Type::None, false } },  // Yoshi's Universal Gravitation
        { "KYGP", { Save::Type::Eeprom,    Gpio::Type::None, false } }   // Yoshi's Universal Gravitation
    };

    for (const auto& [code, overwrite] : kOverwrites)
    {
        if (code == game_code)
            return overwrite;
    }
    return std::nullopt;
}
