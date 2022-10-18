#include "Util.h"
#include <vector>
#include <fstream>
#include <assert.h>
#include <iostream>
#include <iomanip>

std::ofstream debugLog;
std::vector<unsigned char> romFragment; //0x99F8B1 to 99FAB4.
std::vector<unsigned char> ram;
std::vector<unsigned short> staging;
std::vector<unsigned short> decompressed;
int instructionLimit = 20000;
int printedInstructionCount = 0;

void OpenDebugLog()
{
    debugLog.open("output.txt", std::ofstream::out | std::ofstream::trunc);
}

std::vector<unsigned char> LoadBinaryFile8(char const* fileName)
{
    std::vector<unsigned char> result;

    FILE* file{};
    fopen_s(&file, fileName, "rb");
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    result.resize(length);
    fseek(file, 0, SEEK_SET);
    fread(result.data(), 1, length, file);
    fclose(file);

    return result;
}

std::vector<unsigned short> LoadBinaryFile16(char const* fileName)
{
    std::vector<unsigned short> result;

    FILE* file{};
    fopen_s(&file, fileName, "rb");
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    result.resize(length);
    fseek(file, 0, SEEK_SET);
    fread(result.data(), 1, length, file);
    fclose(file);

    return result;
}

void LoadROMFragment()
{
    romFragment = LoadBinaryFile8("rom_fragment.bin");
    staging.resize(0x300);
}

std::vector<int> ramReads;

unsigned short LoadFromRAM(int address)
{
    if (address < 0x7F0000 || address > 0x7F0000 + ram.size())
    {
        __debugbreak();
        return 0xFF;
    }

    ramReads.push_back(address);

    unsigned char ch0 = ram[address - 0x7F0000];
    unsigned char ch1 = ram[address - 0x7F0000 + 1];

    unsigned short result = (ch1 << 8) | ch0;
    return result;
}

unsigned short LoadFromROMFragment(int address)
{
    if (address < 0x99F8B1 || address > 0x99FAB4 - 1)
    {
        __debugbreak();
        return 0xFF;
    }

    int offs = address - 0x99F8B1;

    unsigned char ch0 = romFragment[offs];
    unsigned char ch1 = romFragment[offs+1];
    unsigned short result = (ch1 << 8) | ch0;
    return result;
}


void WriteStagingOutput(int address, unsigned short output)
{
    if (address < 0x7E0500 || address >= 0x7E0800)
    {
        __debugbreak();
        return;
    }

    int offs = address - 0x7E0500;
    int index = offs / 2;
    staging[index] = output;
}

void WriteDecompressedOutput(int address, unsigned short output)
{
    if (address < 0x7F5100 || address > 0x7F7390)
    {
        __debugbreak();
        return;
    }

    int outputOffset = address - 0x7F5100;
    assert(outputOffset % 2 == 0);

    int outputElement = outputOffset / 2;

    decompressed[outputElement] = output;
}

void DebugPrintRegs(unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << " A:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << a;
    debugLog << " X:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << x;
    debugLog << " Y:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << y;
    debugLog << " P:envmxdizc";
}

void DebugPrintFinalize()
{
    debugLog << "\n";
    debugLog.flush();

    ++printedInstructionCount;

    if (printedInstructionCount > instructionLimit)
    {
        exit(0);
    }
}

void DebugPrint(const char* asmText, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << asmText;
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintWithPCAndImm8(unsigned short pc, const char* asmByte, const char* asmOp, unsigned char imm8, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << "$80/";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << pc << " ";
    debugLog << asmByte << ' ' << std::hex << std::setw(2) << std::setfill('0') << (int)imm8 << "       ";
    debugLog << asmOp << " #$" << std::hex << std::setw(2) << std::setfill('0') << (int)imm8 << "               ";
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintWithIndex(const char* asmPrefix, unsigned short index, unsigned short a, unsigned short x, unsigned short y, bool longAddress)
{
    debugLog << asmPrefix;
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << index;
    debugLog << "]";
    if (!longAddress)
    {
        debugLog << "  ";
    }
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintWithPC(unsigned short pc, const char* asmText, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << "$80/";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << pc << " ";
    debugLog << asmText;
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrintWithPCAndIndex(unsigned short pc, const char* asmText, unsigned short index, unsigned short a, unsigned short x, unsigned short y)
{
    debugLog << "$80/";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << pc << " ";
    debugLog << asmText;
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << index;
    debugLog << "]  ";
    DebugPrintRegs(a, x, y);
    DebugPrintFinalize();
}

void DebugPrint85F4(unsigned short a, unsigned short x, unsigned short y)
{
    DebugPrintWithIndex("$9B/85F4 B1 0C       LDA ($0C),y[$7F:", y, a, x, y);
}

void DebugPrint864B(unsigned short a, unsigned short x, unsigned short y)
{
    DebugPrintWithIndex("$9B/864B 91 10       STA ($10),y[$7F:", 0x5100 + y, a, x, y);
}

void DebugPrint8655(unsigned short a, unsigned short x, unsigned short y)
{
    DebugPrintWithIndex("$9B/8655 91 10       STA ($10),y[$7F:", 0x5100 + y, a, x, y);
}

unsigned short ExchangeShortHighAndLow(unsigned short s)
{
    unsigned short part0 = s & 0x00FF;
    unsigned short part1 = s & 0xFF00;
    part0 <<= 8;
    part1 >>= 8;
    s = part0 | part1;
    return s;
}