#pragma once

void OpenDebugLog();

unsigned short LoadFromRAM(int address);
void WriteOutput(int address, unsigned short output);

void DebugPrint(const char* asmText, unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry);
void DebugPrint85F4(unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry);
void DebugPrint864B(unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry);
void DebugPrint8655(unsigned short a, unsigned short x, unsigned short y, bool negative, bool zero, bool carry);