#include "Util.h"
#include <vector>
#include <fstream>
#include <assert.h>
#include <iostream>
#include <iomanip>

std::ofstream debugLog;

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

unsigned short IncLow8(unsigned short s)
{
    unsigned char ch = s & 0xFF;
    ++ch;
    s = ch;
    return s;
}

void RotateLeft(unsigned short* pS, bool* pC)
{
    bool c = *pS >= 0x8000;
    *pS = *pS << 1;
    if (*pC)
    {
        *pS |= 0x1;
    }
    *pC = c;
}