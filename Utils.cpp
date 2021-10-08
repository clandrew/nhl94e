#include "pch.h"
#include "Utils.h"

static const unsigned int sc_colorBufferCellsPerRow = 16;

unsigned int GetFileOffsetFromROMAddress(unsigned int romAddress)
{
    if (romAddress >= 0x028000 && romAddress <= 0x02FFFF)
    {
        return 0x10200 + (romAddress - 0x028000);
    }
    else if (romAddress >= 0xA08000 && romAddress <= 0xA0FFFF)
    {
        return 0x100200 + (romAddress - 0xA08000);
    }
    else if (romAddress >= 0xA18000 && romAddress <= 0xA1FFFF)
    {
        return 0x108200 + (romAddress - 0xA18000);
    }
    else if (romAddress >= 0xA28000 && romAddress <= 0xA2FFFF)
    {
        return 0x110200 + (romAddress - 0xA28000);
    }
    else if (romAddress >= 0xA38000 && romAddress <= 0xA3FFFF)
    {
        return 0x118200 + (romAddress - 0xA38000);
    }
    else if (romAddress >= 0xA48000 && romAddress <= 0xA4FFFF)
    {
        return 0x120200 + (romAddress - 0xA48000);
    }
    else
    {
        assert(false); // Unexpected value
        return 0xFFFFFFFF;
    }
}

long GetFileSize(FILE* file)
{
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    return fileSize;
}

unsigned int GetByteIndexForRow(
    unsigned int cellNumber,
    unsigned int indexWithinCell,
    unsigned char* xWithinCell = nullptr)
{
    assert(indexWithinCell < 64);
    const unsigned int bytesPerCell = 16;
    const unsigned int cellX = indexWithinCell % 8;
    const unsigned int cellY = indexWithinCell / 8;
    const unsigned int srcIndBase = (cellNumber * bytesPerCell) + (cellY * 2);
    if (xWithinCell)
    {
        *xWithinCell = cellX;
    }
    return srcIndBase;
}

void SetCellRowData(
    std::vector<unsigned char>& buffer,
    unsigned int cellNumber,
    unsigned int indexWithinCell,
    unsigned char r0,
    unsigned char r1)
{
    const unsigned int srcIndBase = GetByteIndexForRow(cellNumber, indexWithinCell);
    assert(srcIndBase + 1 < buffer.size());

    buffer[srcIndBase + 0] = r0;
    buffer[srcIndBase + 1] = r1;
}

void GetCellRowData(
    std::vector<unsigned char> const& buffer,
    unsigned int cellNumber,
    unsigned int indexWithinCell,
    unsigned char* r0,
    unsigned char* r1,
    unsigned char* xWithinCell)
{
    const unsigned int srcIndBase = GetByteIndexForRow(cellNumber, indexWithinCell, xWithinCell);
    *r0 = buffer[srcIndBase + 0];
    *r1 = buffer[srcIndBase + 1];
}

int SnesB5G5R5ToR8B8G8(unsigned short snesFmt)
{
    unsigned short r5 = snesFmt & 0x1F;
    snesFmt >>= 5;
    unsigned short g5 = snesFmt & 0x1F;
    snesFmt >>= 5;
    unsigned short b5 = snesFmt & 0x1F;

    int r8 = r5 * 8;
    int g8 = g5 * 8;
    int b8 = b5 * 8;

    int rgb = r8;
    rgb <<= 8;
    rgb |= g8;
    rgb <<= 8;
    rgb |= b8;

    return rgb;
}

unsigned short R8B8G8ToSnesB5G5R5(int rgbFmt)
{
    unsigned short b8 = rgbFmt & 0xFF;
    rgbFmt >>= 8;
    unsigned short g8 = rgbFmt & 0xFF;
    rgbFmt >>= 8;
    unsigned short r8 = rgbFmt & 0xFF;

    int r5 = r8 / 8;
    int g5 = g8 / 8;
    int b5 = b8 / 8;

    unsigned short snes = b5;
    snes <<= 5;
    snes |= g5;
    snes <<= 5;
    snes |= r5;

    return snes;
}