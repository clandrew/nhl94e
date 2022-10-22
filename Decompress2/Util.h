#pragma once
#include <vector>

void OpenDebugLog();

std::vector<unsigned char> LoadBinaryFile8(char const* fileName);

void DebugPrint(const char* asmText, unsigned short a, unsigned short x, unsigned short y);
void DebugPrintWithIndex(const char* asmPrefix, unsigned short index, unsigned short a, unsigned short x, unsigned short y, bool longAddress=false);

void DebugPrintWithPC(unsigned short pc, const char* asmText, unsigned short a, unsigned short x, unsigned short y);
void DebugPrintWithPCAndIndex(unsigned short pc, const char* asmText, unsigned short index, unsigned short a, unsigned short x, unsigned short y);
void DebugPrintWithPCAndImm8(unsigned short pc, const char* asmByte, const char* asmOp, unsigned char imm8, unsigned short a, unsigned short x, unsigned short y);

unsigned short ExchangeShortHighAndLow(unsigned short s);

unsigned short IncLow8(unsigned short s);
void RotateLeft(unsigned short* pS, bool* pC);