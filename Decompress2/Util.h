#pragma once
#include <vector>

void OpenDebugLog();
void LoadROMFragment();

unsigned short LoadFromRAM(int address);
unsigned short LoadFromROMFragment(int address);
std::vector<unsigned char> LoadBinaryFile8(char const* fileName);

void WriteStagingOutput(int address, unsigned short output);

void WriteDecompressedOutput(int address, unsigned short output);

void DebugPrint(const char* asmText, unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry);
void DebugPrintWithAddress(const char* asmPrefix, unsigned short index, unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry, bool longAddress=false);
void DebugPrint85F4(unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry);
void DebugPrint864B(unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry);
void DebugPrint8655(unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry);
void DebugPrintNewline();

unsigned short ExchangeShortHighAndLow(unsigned short s);