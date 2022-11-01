#pragma once

enum class Team
{
    Anaheim = 0x0,
    Boston = 0x1,
    Buffalo = 0x2,
    Calgary = 0x3,
    Chicago = 0x4,
    Dallas = 0x5,
    Detroit = 0x6,
    Edmonton = 0x7,
    Florida = 0x8,
    Hartford = 0x9,
    LAKings = 0xA,
    Montreal = 0xB,
    NewJersey = 0xC,
    NYIslanders = 0xD,
    NYRangers = 0xE,
    Ottawa = 0xF,
    Philly = 0x10,
    Pittsburgh = 0x11,
    Quebec = 0x12,
    SanJose = 0x13,
    StLouis = 0x14,
    TampaBay = 0x15,
    Toronto = 0x16,
    Vancouver = 0x17,
    Washington = 0x18,
    Winnipeg = 0x19,
    AllStarsEast = 0x1A,
    AllStarsWest = 0x1B,
    Count
};

union Mem16
{
    unsigned short Data16;
    struct
    {
        unsigned char Low8;
        unsigned char High8;
    };
};

void Decompress_Slow_Init();
void Decompress_Slow_Run(int teamIndex, int playerIndex);

bool Decompress_Fast_Init();
void Decompress_Fast_Run(int teamIndex, int playerIndex);