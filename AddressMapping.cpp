#include "pch.h"

int ROMAddressToFileOffset(int romAddress)
{
    assert(romAddress >= 0x800000); // Lower ones haven't been tested

    int offsetWithinSection = romAddress % 0x8000;

    int section = (romAddress - 0x800000) >> 16;

    int offset = section * 0x8000 + offsetWithinSection;

    return offset;
}

int FileOffsetToROMAddress(int fileOffset)
{
    int section = fileOffset / 0x8000;  

    int offsetWithinSection = fileOffset % 0x8000;

    int addr = 0x800000 + (section * 0x10000) + 0x8000 + offsetWithinSection;

    return addr;
}

bool ROMAddressRangeCrossesPageBoundary(int beginAddr, int endAddr)
{
    return beginAddr >> 16 != endAddr >> 16;
}