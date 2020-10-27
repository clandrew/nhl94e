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

std::vector<unsigned char> LoadBytesFromFile(const wchar_t* fileName)
{
    std::wstring sourcePath = fileName;

    std::vector<unsigned char> destFile;

    FILE* file = {};
    _wfopen_s(&file, sourcePath.c_str(), L"rb");
    long retrievedFileSize = GetFileSize(file);

    destFile.resize(retrievedFileSize);
    fread(&destFile[0], 1, retrievedFileSize, file);
    fclose(file);

    return destFile;
}

void SaveBytesToFile(const wchar_t* fileName, std::vector<unsigned char>& romData)
{
    std::wstring path = fileName;

    FILE* file;
    _wfopen_s(&file, path.c_str(), L"wb");
    fwrite(&romData[0], 1, romData.size(), file);
    fclose(file);
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
