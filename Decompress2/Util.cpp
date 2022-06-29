#include "Util.h"
#include <vector>
#include <fstream>
#include <assert.h>
#include <iostream>
#include <iomanip>

std::ofstream debugLog;
std::vector<unsigned char> romFragment; //0x99F8B1 to 99FAB4.
std::vector<unsigned char> ram;
std::vector<unsigned short> decompressed;
int instructionLimit = 60503;
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

void WriteOutput(int address, unsigned short output)
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

void DebugPrintRegs(unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry)
{
    bool preserveFlags = false;
    if (!preserveFlags)
    {
        negative = false;
        zero = false;
        carry = false;
    }

    debugLog << " A:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << a;
    debugLog << " X:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << x;
    debugLog << " Y:" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << y;
    debugLog << " P:e";
    debugLog << (negative ? "N" : "n");
    debugLog << "vmxdi";
    debugLog << (zero ? "Z" : "z");
    debugLog << (carry ? "C" : "c");
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

void DebugPrint(const char* asmText, unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry)
{
    debugLog << asmText;
    DebugPrintRegs(a, x, y, negative, zero, carry);
    DebugPrintFinalize();
}

void DebugPrint85F4(unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry)
{
    debugLog << "$9B/85F4 B1 0C       LDA ($0C),y[$7F:";
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << y;
    debugLog << "]  ";
    DebugPrintRegs(a, x, y, negative, zero, carry);
    DebugPrintFinalize();
}

void DebugPrint864B(unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry)
{
    debugLog << "$9B/864B 91 10       STA ($10),y[$7F:";
    unsigned short offset = 0x5100 + y;
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << offset;
    debugLog << "]  ";
    DebugPrintRegs(a, x, y, negative, zero, carry);
    DebugPrintFinalize();
}

void DebugPrint8655(unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry)
{
    debugLog << "$9B/8655 91 10       STA ($10),y[$7F:";
    unsigned short offset = 0x5100 + y;
    debugLog << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << offset;
    debugLog << "]  ";
    DebugPrintRegs(a, x, y, negative, zero, carry);
    DebugPrintFinalize();
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